// Compile on Windows: gcc httpserver.c -DTLS_AMALGAMATION -lws2_32 -o httpserver.exe
// Other OSes: gcc httpserver.c -DTLS_AMALGAMATION -o httpserver

#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <winsock2.h>
#define socklen_t int
#define sleep(x)    Sleep(x*1000)
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include "tlse.c"


#include "process_command.h"


#define SERVER_VERSION "2017.08.14.0"

#define port 443
#define MAX_CONNECTIONS         1024
#define BUFFER_SIZE             0xFFFF
#define LINE_SIZE               8192
#define DEBUG_INFO(msg, data)   fprintf(stderr, msg, data);

enum {
	METHOD_UNKNOWN,
	METHOD_GET,
	METHOD_POST,
	METHOD_PUT,
	METHOD_DELETE
};

struct HTTPConnection {
	SSL *context;
	int socket;
	int timestamp;
	unsigned char is_websocket;
};

static struct HTTPConnection connections[MAX_CONNECTIONS];
static int connection_count = 0;

int __tls_ssl_private_send_pending(int client_sock, struct TLSContext *context);

static signed char is_little_endian = 1;

FILE *f_log;
//--------------------------------------------------------------------
uint64_t htonll2(uint64_t a) 
{
	if (is_little_endian)
		a = ((a & 0x00000000000000FFULL) << 56) |
		((a & 0x000000000000FF00ULL) << 40) |
		((a & 0x0000000000FF0000ULL) << 24) |
		((a & 0x00000000FF000000ULL) << 8) |
		((a & 0x000000FF00000000ULL) >> 8) |
		((a & 0x0000FF0000000000ULL) >> 24) |
		((a & 0x00FF000000000000ULL) >> 40) |
		((a & 0xFF00000000000000ULL) >> 56);
	return a;
}
//--------------------------------------------------------------------
static const char b64_table[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};
//--------------------------------------------------------------------
char *b64_encode(const unsigned char *src, int len) 
{
	int i = 0;
	int j = 0;
	char *enc = NULL;
	int size = 0;
	unsigned char buf[4];
	unsigned char tmp[3];

	while (len--) {
		tmp[i++] = *(src++);

		if (3 == i) {
			buf[0] = (tmp[0] & 0xfc) >> 2;
			buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
			buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
			buf[3] = tmp[2] & 0x3f;

			enc = (char *)realloc(enc, size + 4);
			for (i = 0; i < 4; ++i)
				enc[size++] = b64_table[buf[i]];

			i = 0;
		}
	}

	if (i > 0) {
		for (j = i; j < 3; ++j)
			tmp[j] = '\0';

		buf[0] = (tmp[0] & 0xfc) >> 2;
		buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
		buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
		buf[3] = tmp[2] & 0x3f;

		for (j = 0; (j < i + 1); ++j) {
			enc = (char *)realloc(enc, size + 1);
			enc[size++] = b64_table[buf[j]];
		}

		while ((i++ < 3)) {
			enc = (char *)realloc(enc, size + 1);
			enc[size++] = '=';
		}
	}

	enc = (char *)realloc(enc, size + 1);
	enc[size] = '\0';

	return enc;
}
//--------------------------------------------------------------------
int set_connection(int socket, SSL *server_ctx) 
{
	int i;
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (!connections[i].socket) {
			DEBUG_INFO("Connected slot #%i\n", i + 1);
			connections[i].socket = socket;
			connections[i].context = SSL_new(server_ctx);
			connections[i].timestamp = time(NULL);
			connections[i].is_websocket = 0;
			SSL_set_fd(connections[i].context, socket);
			connection_count++;
			return 1;
		}
	}
	return 0;
}
//--------------------------------------------------------------------
int close_connection(int socket) 
{
	int i;
	if (socket <= 0)
		return 0;

	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (connections[i].socket == socket) {
			DEBUG_INFO("Disconnected slot #%i\n", i + 1);
			if (connections[i].context) {
				SSL_shutdown(connections[i].context);
				SSL_free(connections[i].context);
			}
#ifdef _WIN32
			shutdown(connections[i].socket, SD_BOTH);
			closesocket(connections[i].socket);
#else
			shutdown(connections[i].socket, SHUT_RDWR);
			close(connections[i].socket);
#endif
			connections[i].socket = 0;
			connections[i].context = NULL;
			connections[i].is_websocket = 0;
			connection_count--;
			return 1;
		}
	}
	return 0;
}
//--------------------------------------------------------------------
// very inefficient
// it would be better to have multiple file descriptors per iteration (FD_SET(connection[x].socket ...)
// or even better, use epoll/kqueue
int data_pending(int socket, int t_out) 
{
#ifdef _WIN32
	struct timeval timeout;
	timeout.tv_sec = 0;
	if (t_out < 0)
		timeout.tv_usec = 0;
	else
		timeout.tv_usec = (int)t_out * 1000;
	fd_set socks;

	FD_ZERO(&socks);
	FD_SET((int)socket, &socks);

	int sel_val = select(FD_SETSIZE, &socks, 0, 0, &timeout);

	return (sel_val != 0);
#else
	struct pollfd ufds[1];
	ufds[0].fd = socket;
	ufds[0].events = POLLIN;

	if (t_out < 0)
		t_out = 0;

	return poll(ufds, 1, (int)t_out);
#endif
}
//--------------------------------------------------------------------
int parse_header(const char *buffer, int size, unsigned char *is_websocket, int *use_method, char *s_url, char *web_socket_key) 
{
	char method[20];
	char url[LINE_SIZE];
	char header[LINE_SIZE];
	int i;
	int line = 0;
	int space = 0;
	int parameter_size = 0;
	*use_method = METHOD_UNKNOWN;
	int value_pos = 0;
	for (i = 0; i < size; i++) {
		char b = buffer[i];
		if (b == '\r') {
			if ((line) && (strstr(header, "sec-websocket-key"))) {
				while ((header[value_pos]) && (header[value_pos] == ' '))
					value_pos++;

				strcpy(web_socket_key, header + value_pos);
				*is_websocket = 1;
			}
			i++;
			line++;
			parameter_size = 0;
			value_pos = 0;
			continue;
		}
		else
			if (b == ':')
				value_pos = parameter_size + 1;

		if (line) {
			if (value_pos)
				header[parameter_size] = b;
			else
				header[parameter_size] = tolower(b);
			parameter_size++;
			header[parameter_size] = 0;
			if (parameter_size >= sizeof(header) - 1)
				return 0;
		}
		else {
			if (b == ' ') {
				switch (space) {
				case 0:
					// method;
					if (strcmp(method, "GET")) {
						DEBUG_INFO("Unsupported method %s\n", method);
						return 0;
					}
					*use_method = METHOD_GET;
					break;
				case 1:
					if (!strcmp(url, "/"))
						strcpy(url, "/index.html");

					strcpy(s_url, url);
					if (strstr(url, "..")) {
						DEBUG_INFO("Unsafe URL %s\n", url);
						return 0;
					}
					break;
				}
				space++;
				parameter_size = 0;
				continue;
			}
			switch (space) {
			case 0:
				method[parameter_size] = toupper(b);
				parameter_size++;
				method[parameter_size] = 0;
				if (parameter_size >= sizeof(method) - 1)
					return 0;
				break;
			case 1:
				url[parameter_size] = b;
				parameter_size++;
				url[parameter_size] = 0;
				if (parameter_size >= sizeof(url) - 1)
					return 0;
				break;
			case 2:
				// http version ignored
				break;
			}
		}
	}
	char *question_mark_ptr = strchr(s_url, '?');
	if (question_mark_ptr)
		s_url[question_mark_ptr - s_url] = 0;
	return 1;
}
//--------------------------------------------------------------------
long WS_get_size(const char *buf, int size, int *type, int *masked, int *fin, int *offset) 
{
	*type = -1;
	*offset = 0;
	if (fin)
		*fin = 0;

	if (size <= 2)
		return -1;

	if (fin)
		*fin = buf[0] & 0x80;

	*type = buf[0] & 0x0F;
	switch (*type) {
	case 0x0:
		// continuation frame
		break;

	case 0x1:
		// text frame;
		break;

	case 0x0D:
		// private rtc-extension frame
	case 0x02:
		// binary frame
		break;

	case 0x08:
		// connection close
		return -1;

	case 0x09:
		// ping
		*type = 1;
		break;

	case 0x0A:
		// pong
		*type = 1;
		break;

	default:
		// unknown message type, drop it
		break;
	}

	char s_buf = buf[1] & 0x7F;
	*masked = buf[1] & 0x80;
	buf += 2;
	*offset += 2;
	switch (s_buf) {
	case 126:
		if (size <= 4)
			return -1;
		*offset += 2;
		return ntohs(*(unsigned short *)buf);

	case 127:
		if (size <= 10)
			return -1;
		*offset += 8;
		return (long)htonll2(*(uint64_t *)buf);

	default:
		if (s_buf <= 0x7D)
			return s_buf;
	}
	return -1;
}
//--------------------------------------------------------------------
int ws_send(struct HTTPConnection *connection, const char *buffer, int size) 
{
	char *ws_buffer = (char *)malloc(size + 20);
	if (!ws_buffer)
		return 0;

	uint64_t tmp = size;
	int  size_len = 0;

	ws_buffer[0] = '\x82';
	if (tmp <= 0x7D) {
		ws_buffer[1] = tmp;
		size_len++;
	}
	else
		if (tmp < 0xFFFF) {
			ws_buffer[1] = 0x7E;
			*(unsigned short *)&ws_buffer[2] = htons((unsigned short)tmp);
			size_len += 3;
		}
		else {
			ws_buffer[1] = 0x7F;
			*(uint64_t *)ws_buffer = htonll2(tmp);
			size_len += 9;
		}
		memcpy(ws_buffer + size_len + 1, buffer, size);
		int out_size = size + size_len + 1;
		char *ptr = ws_buffer;
		do {
			int written = SSL_write(connection->context, ptr, out_size);
			if (written <= 0) {
				free(ws_buffer);
				return written;
			}
			out_size -= written;
			ptr += written;
		} while (out_size > 0);
		free(ws_buffer);
		return size + size_len + 1;
}
//--------------------------------------------------------------------
void on_ws_data(struct HTTPConnection *connection, const char *buffer, int size) 
{
	DEBUG_INFO("WS DATA: %s\n", buffer);
	//	if (size == 2)
	int result = process_command(buffer[0], buffer[1]);
	if (result == E_OK) {
		if (buffer[1] == CAPTURE_HEAD_CAMERA || buffer[1] == CAPTURE_LEFT_ARM_CAMERA) {
			char tmp_buffer[2];
			sprintf(tmp_buffer, "%d", E_OK);
			ws_send(connection, tmp_buffer, 1);
		}
	}
	else {
		char tmp_buffer[2];
		sprintf(tmp_buffer, "%d", result);
		ws_send(connection, tmp_buffer, 1);
	}
}
//--------------------------------------------------------------------
int on_data_received(struct HTTPConnection *connection, const char *buffer, int size)
{
	char work_buffer[BUFFER_SIZE];
	char pong[6];
	int i;
	// return 1 = connection close;
	if (connection->is_websocket) {
		while (size > 0) {
			int type;
			int masked;
			int fin;
			int offset = 0;
			int ws_size = (int)WS_get_size(buffer, size, &type, &masked, &fin, &offset);
			DEBUG_INFO("WS PACKET SIZE: %i\n", (int)ws_size);
			if (ws_size < 0)
				return 1;
			switch (type) {
			case 0x00:
			case 0x01:
			case 0x02:
				// text/continuation/binary frame
				if (ws_size > size - offset) {
					DEBUG_INFO("Connection dropped. Packet boundary not implemented.\n", "");
					return 1;
				}
				if (masked) {
					if (ws_size > size - offset - 4)
						return 1;
					const char *mask = &buffer[offset];
					offset += 4;
					int len = ws_size;
					if (len > sizeof(work_buffer) - 1)
						len = sizeof(work_buffer) - 1;
					for (i = 0; i < ws_size; i++)
						work_buffer[i] = buffer[i + offset] ^ mask[i % 4];
					work_buffer[i] = 0;
				}
				on_ws_data(connection, work_buffer, ws_size);
				break;
			case 0x08:
				return 1;
			case 0x09:
				// send pong
				DEBUG_INFO("Received ping\n", "");
				pong[0] = 0x8A;
				pong[1] = 0x04;
				pong[2] = 'P';
				pong[3] = 'o';
				pong[4] = 'n';
				pong[5] = 'g';
				SSL_write(connection->context, pong, 6);
				break;
			}
			offset += ws_size;
			size -= offset;
			buffer += offset;
		}
	}
	else {
		DEBUG_INFO("RECEIVED REQUEST:\n%s\n", buffer);
		int method;
		char url[LINE_SIZE];
		char websocket_key[LINE_SIZE + 64];
		url[0] = 0;
		websocket_key[0] = 0;
		if (!parse_header(buffer, size, &connection->is_websocket, &method, url, websocket_key)) {
			DEBUG_INFO("Invalid header\n", "");
			return 1;
		}
		if (connection->is_websocket) {
			char *msg = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\nAccess-Control-Allow-Headers:x-websocket-protocol\r\nSec-WebSocket-Protocol: data\r\n\r\n";
			strcat(websocket_key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
			unsigned char tmp[20];
			hash_state md;
			sha1_init(&md);
			sha1_process(&md, (unsigned char *)websocket_key, strlen(websocket_key));
			sha1_done(&md, tmp);
			work_buffer[0] = 0;
			char *key = b64_encode(tmp, 20);
			snprintf(work_buffer, sizeof(work_buffer), msg, key);
			DEBUG_INFO("UPGRADING TO WEBSOCKET, KEY: '%s'\n", key);
			free(key);
			SSL_write(connection->context, work_buffer, strlen(work_buffer));
			return 0;
		}
		else {
			char *path_mask = "c:/Mihai/Dropbox/jenny 5/jenny5-html5-controller/client/www/%s";
			char full_path[LINE_SIZE];
			full_path[0] = 0;
			snprintf(full_path, sizeof(full_path), path_mask, url);
			FILE *f = fopen(full_path, "rb");
			if (f) {
				// DEBUG_INFO("GET %s\n", full_path);
				fseek(f, 0, SEEK_END);
				int filesize = ftell(f);
				fseek(f, 0, SEEK_SET);
				char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %i\r\nConnection: close\r\nCache-Control: max-age=0, no-cache\r\n\r\n";
				int wsize = snprintf(work_buffer, sizeof(work_buffer), msg, filesize);
				if (SSL_write(connection->context, work_buffer, wsize) == wsize) {
					int read_size;
					do {
						// must read less than 64k!! (maximum TLS packet size)
						read_size = fread(work_buffer, 1, LINE_SIZE, f);
						if (read_size > 0) {
							if (SSL_write(connection->context, work_buffer, read_size) <= 0)
								break;
						}
					} while (read_size > 0);
				}
				fclose(f);
			}
			else {
				DEBUG_INFO("%s not found\n", full_path);
				char *msg = "HTTP/1.1 404 Not found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\nNot found";
				SSL_write(connection->context, msg, strlen(msg));
			}
		}
		return 1;
	}
	return 0;
}//--------------------------------------------------------------------
int iterate_socket(struct HTTPConnection *connection) 
{
	// use low level api
	char buffer[BUFFER_SIZE];
	int size = recv(connection->socket, (char *)buffer, sizeof(buffer), 0);
	if (size > 0) {
		if (tls_consume_stream(connection->context, (const unsigned char *)buffer, size, tls_default_verify) >= 0) {
			if (__tls_ssl_private_send_pending(connection->socket, connection->context) < 0)
				size = -1;
		}
		else
			size = -1;
	}
	if (size <= 0) {
		// disconnected or error
		close_connection(connection->socket);
		return 0;
	}
	if (tls_established(connection->context)) {
		// reserve one character for null-terminating the buffer
		size = tls_read(connection->context, (unsigned char *)buffer, sizeof(buffer) - 1);
		if (size) {
			if (size > 0)
				buffer[size] = 0;
			if ((size < 0) || (on_data_received(connection, buffer, size))) {
				close_connection(connection->socket);
				return 0;
			}
		}
	}
	return 1;
}
//--------------------------------------------------------------------
int iterate_sockets() 
{
	int socket_count = connection_count;
	int iterations = 0;
	int i;
	if (socket_count > 0) {
		for (i = 0; i < MAX_CONNECTIONS; i++) {
			if ((connections[i].socket) && (data_pending(connections[i].socket, 0))) {
				iterations++;
				if (!iterate_socket(&connections[i])) {
					// disconnected
					socket_count--;
				}
				socket_count--;
				if (socket_count <= 0)
					break;
			}
		}
	}
	return iterations;
}
//--------------------------------------------------------------------
int main(int argc, char *argv[]) 
{
	int socket_desc, client_sock, read_size;
	socklen_t c;
	struct sockaddr_in server, client;

	char log_filename[1000];
	current_time_to_string(log_filename);
	strcat(log_filename, ".txt");

	f_log = fopen(log_filename, "w");

	if (!f_log) {
		printf("Cannot write log file! Press Enter to terminate.");
		getchar();
		return 1;
	}

	char message[1000];

	sprintf(message, "Server version: %s\n", SERVER_VERSION);
	print_message(stdout, message);
	print_message(f_log, message);


	memset(connections, 0, sizeof(connections));	

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
	signal(SIGPIPE, SIG_IGN);
#endif

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		sprintf(message, "Could not create socket");
		print_message(stdout, message);
		print_message(f_log, message);
		fclose(f_log);


		return 0;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	int enable = 1;
	setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(int));

	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		sprintf(message, "bind failed. Error\n");
		print_message(stdout, message);
		print_message(f_log, message);

		fclose(f_log);
		return 1;
	}

	listen(socket_desc, MAX_CONNECTIONS);

	c = sizeof(struct sockaddr_in);

	unsigned int size;

	SSL *server_ctx = SSL_CTX_new(SSLv3_server_method());
	if (!server_ctx) {
		sprintf(message, "Error creating server context\n");
		print_message(stdout, message);
		print_message(f_log, message);

		fclose(f_log);

		return -1;
	}

	SSL_CTX_use_certificate_file(server_ctx, "c:/Mihai/Dropbox/jenny 5/jenny5-html5-controller/client/testcert/server.cer", SSL_SERVER_RSA_CERT);
	SSL_CTX_use_PrivateKey_file(server_ctx, "c:/Mihai/Dropbox/jenny 5/jenny5-html5-controller/client/testcert/server.key", SSL_SERVER_RSA_KEY);

	if (!SSL_CTX_check_private_key(server_ctx)) {
		sprintf(message, "Private key not loaded\n");
		print_message(stdout, message);
		print_message(f_log, message);

		fclose(f_log);
		return -2;
	}

	init_robot();


	while (1) {
		int timeout = 100;
		// optimize CPU load
		if (iterate_sockets())
			timeout = 0;

		// wait 100ms (give CPU some time)
		if (data_pending(socket_desc, timeout)) {
			client_sock = accept(socket_desc, (struct sockaddr *)&client, &c);
			if (client_sock <= 0) {
				perror("accept");
				break;
			}
			if (!set_connection(client_sock, server_ctx)) {
				fprintf(stderr, "Too many connections. Consider increasing MAX_CONNECTIONS. Current value is %i\n", (int)MAX_CONNECTIONS);
#ifdef _WIN32
				closesocket(client_sock);
#else
				close(client_sock);
#endif
			}
		}
	}
	SSL_CTX_free(server_ctx);


	return 0;
}
//--------------------------------------------------------------------