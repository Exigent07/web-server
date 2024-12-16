# Web Server Documentation

## Overview
This is a simple HTTP server implemented in C++. The server listens for incoming HTTP requests, processes them, and responds with appropriate HTTP responses. It supports serving static files from a specified root directory, parsing HTTP requests, and constructing HTTP responses.

## Directory Structure
```
/your/path
│
├── assets/                    # Root directory for serving files
├── config/
│   └── conf.json              # Configuration file for the server
├── main.cpp                   # Main entry point for the server
├── request.hpp                # Header file for handling HTTP requests
├── response.hpp               # Header file for handling HTTP responses
└── server.hpp                 # Header file for the server implementation
```

## Configuration
The server reads its configuration from a JSON file (`conf.json`). The configuration specifies the following parameters:

- **port**: The port number the server listens on (default: 8080).
- **serverRoot**: The root directory for serving files (default: `/home/exigent07/CPP/web-server/assets`).
- **errorLog**: The file path for the error log (default: `/var/log/http-server/error.log`).
- **accessLog**: The file path for the access log (default: `/var/log/http-server/access.log`).

### Example `conf.json`
```json
{
    "port": 8080,
    "serverRoot": "/home/exigent07/CPP/web-server/assets",
    "errorLog": "/var/log/http-server/error.log",
    "accessLog": "/var/log/http-server/access.log"
}
```

## Classes and Components

### `Request`
This class represents an HTTP request. It parses the incoming raw HTTP request string and extracts relevant information such as the HTTP method, URI, headers, and body.

- **Methods**:
  - `Request()`: Default constructor.
  - `Request(const std::string& rawRequest)`: Constructor that parses the provided raw HTTP request string.
  - `parse(const std::string& rawRequest)`: Parses the raw HTTP request string into method, URI, headers, and body.
  - `getMethod()`: Returns the HTTP method of the request.
  - `getUri()`: Returns the URI of the request.
  - `getHeaders()`: Returns the headers of the request.
  - `getBody()`: Returns the body of the request.

### `Response`
This class represents an HTTP response. It constructs the response based on the status code, headers, and body content. It also has the ability to send files as the response body.

- **Methods**:
  - `Response()`: Default constructor.
  - `setStatusCode(int code)`: Sets the HTTP status code and corresponding message.
  - `addHeader(const std::string& key, const std::string& value)`: Adds a header to the response.
  - `setBody(const std::string& responseBody)`: Sets the body of the response.
  - `toString()`: Converts the response to a properly formatted HTTP response string.
  - `sendFile(const std::string& filePath)`: Sends a file as the response body.

### `Server`
The `Server` class is responsible for setting up and running the HTTP server. It listens for incoming client requests, processes them, and sends the appropriate responses.

- **Methods**:
  - `Server(const std::string& configPath)`: Constructor that loads the configuration from the specified JSON file (default: `/etc/http-server/conf.json`).
  - `loadConfig(const std::string& configPath)`: Loads the server configuration from a JSON file.
  - `startListening()`: Starts the server, listens for incoming connections, and processes HTTP requests.

## Compilation and Usage

1. **Build the Project**:
   To compile the project, use the following `g++` command:
   ```bash
   g++ -o web-server main.cpp server.cpp request.cpp response.cpp -ljsoncpp
   ```

2. **Run the Server**:
   You can run the server using:
   ```bash
   ./web-server
   ```

3. **Configuration**:
   Modify the `conf.json` file to configure the server settings like the port, server root directory, and log files.

4. **Accessing the Server**:
   Once the server is running, you can access it by navigating to `http://localhost:8080/` (or the port specified in the configuration file).

5. **Logs**:
   - **Error Log**: Logs server errors.
   - **Access Log**: Logs all incoming client requests.

## Dependencies
This project uses the `jsoncpp` library for parsing JSON configuration files. Ensure that the library is installed before compiling.

To install `jsoncpp`, use the following command (for Ubuntu/Debian):
```bash
sudo apt-get install libjsoncpp-dev
```

## Example Request and Response

- **Request**:
  ```
  GET /index.html HTTP/1.1
  Host: localhost:8080
  Connection: keep-alive
  ```

- **Response**:
  ```
  HTTP/1.1 200 OK
  Content-Type: text/html
  Content-Length: 1234

  <html>...</html>
  ```