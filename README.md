# Jenny5 HTML5 client and websocket server

Control Jenny 5 from any browser.

## How to compile the server

Create a C++ project and all c/cpp files from the *server* folder.

Also add all cpp files from the *src* folder of this repository:

[https://github.com/jenny5-robot/jenny5-control-module](https://github.com/jenny5-robot/jenny5-control-module)

Use the following flag for compilation: -DTLS_AMALGAMATION

On Windows add lws2_32 library to the project.

## How to run

1. Start the server

2. Open a browser and type the address of the PC running the server (with https).

3. Enjoy