# Jenny5 HTML5 client and websocket server

Control Jenny 5 from any browser.

## How to compile the server

Create a C++ project and all c/cpp files from the *server* folder.

Also add all cpp files from the *src* folder of the **Scufy Lib** repository:

[https://github.com/jenny5-robot/Scufy-Lib](https://github.com/jenny5-robot/Scufy-Lib)

Use the following flag for compiling: -DTLS_AMALGAMATION

On Windows add *ws2_32.lib* library to the project.

## How to run

1. Start the server

2. Open a browser and type the IP address of the PC running the server (with *https*).

3. Enjoy controlling the robot

## Screenshot

![Jenny5 controller screenshot](https://github.com/jenny5-robot/jenny5-html5/blob/master/jenny5_controller_screenshot.png "Jenny 5 HTML5 controller screenshot")