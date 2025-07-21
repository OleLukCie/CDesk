# CDesk

### Project Overview
CDesk is a simple desktop sharing application consisting of a client and a server. The client captures the desktop screen and sends it as an image to the server, and the server receives these images and displays them in a window.

### Features
- **Desktop Capture**: The client can capture the desktop screen in real-time.
- **Image Transmission**: The captured desktop images are sent to the server as binary files.
- **Image Display**: The server receives the images and displays them in a window.

### Dependencies
- **EasyX**: Used for graphical interface and image operations.
- **Winsock**: Used for network communication.

### Compilation and Execution
#### Client
1. Compile the file using a C++ compiler (e.g., Visual Studio 2022).
2. Run the generated executable file.

#### Server
1. Compile the file using a C++ compiler (e.g., Visual Studio 2022).
2. Run the generated executable file.

### Notes
- Ensure that the client and server are on the same network, and the IP address and port number of the server are correctly configured in the code.
- The port number used in the client and server code is `8080`. Please make sure this port is not occupied by other applications.

### Code Structure
Client code responsible for capturing the desktop screen and sending images.
Server code responsible for receiving and displaying images.
