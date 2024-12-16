#ifndef SERVER_HPP
#define SERVER_HPP

#include <json/value.h>
#include <json/reader.h>
#include <request.hpp>
#include <response.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define DEFAULT_CONFIG_PATH "/etc/http-server/conf.json"

/**
 * @brief Represents the HTTP server and its configuration.
 * 
 * The Server class initializes the server configuration, loads configuration 
 * data from a JSON file, and provides the foundation for starting and managing 
 * the HTTP server.
 */
class Server {
private:
    int port;                      /** Port number the server listens on */
    int serverSocket;              /** Socket descriptor for the server */
    std::string serverRoot;        /** Root directory for serving files */
    std::string errorLog;          /** Path to the error log file */
    std::string accessLog;         /** Path to the access log file */
    bool isRunning;                /** Indicates whether the server is running */

    /**
     * @brief Loads server configuration from a JSON file.
     * 
     * Reads the configuration file and updates the server parameters such as port, 
     * server root directory, and log file paths. If the file cannot be read or parsed, 
     * default values are used.
     * 
     * @param configPath Path to the JSON configuration file.
     */
    void loadConfig(const std::string& configPath);

public:
    /**
     * @brief Starts the server and begins listening for client requests.
     * 
     * Creates a socket, binds it to the specified port, and listens for incoming 
     * connections. Once a connection is accepted, it starts handling requests.
     */
    void startListening();

    /**
     * @brief Constructs a Server object.
     * 
     * Initializes the server with default values and optionally loads configuration 
     * from a specified JSON file.
     * 
     * @param configPath Path to the configuration file (default: DEFAULT_CONFIG_PATH).
     */
    Server(const std::string& configPath = DEFAULT_CONFIG_PATH);

    /**
     * @brief Destructor for the Server object.
     */
    ~Server();
};

// Implementation

Server::Server(const std::string& configPath) : port(80), serverRoot("/var/www/html/"),
                                                errorLog("/var/log/error_log"), 
                                                accessLog("/var/log/access_log"),
                                                isRunning(false) {
    loadConfig(configPath);
}

Server::~Server() {
    if (serverSocket != -1) {
        close(serverSocket);  // Close the socket when the server object is destroyed
    }
}

// Loads a config file from custom path
void Server::loadConfig(const std::string& configPath) {
    std::ifstream configFile(configPath, std::ifstream::binary);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open configuration file: " << configPath << std::endl;
        return;
    }

    Json::Value config;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    if (!Json::parseFromStream(readerBuilder, configFile, &config, &errs)) {
        std::cerr << "Error parsing configuration file: " << errs << std::endl;
        return;
    }

    if (config.isMember("port")) port = config["port"].asInt();
    if (config.isMember("serverRoot")) serverRoot = config["serverRoot"].asString();
    if (config.isMember("errorLog")) errorLog = config["errorLog"].asString();
    if (config.isMember("accessLog")) accessLog = config["accessLog"].asString();

    std::cout << "Configuration Loaded:" << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Server Root: " << serverRoot << std::endl;
    std::cout << "Error Log: " << errorLog << std::endl;
    std::cout << "Access Log: " << accessLog << std::endl;
}

// Start listening for incoming connections
void Server::startListening() {
    // Create the server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error: Could not create socket" << std::endl;
        return;
    }

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Zero out the structure
    memset(&serverAddr, 0, sizeof(serverAddr));

    // Set up the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Listen on all network interfaces
    serverAddr.sin_port = htons(port);        // Use the specified port

    // Bind the socket to the address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error: Could not bind to port " << port << std::endl;
        close(serverSocket);
        return;
    }

    // Start listening for incoming connections
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Error: Could not listen on port " << port << std::endl;
        close(serverSocket);
        return;
    }

    std::cout << "Server listening on port " << port << "..." << std::endl;
    isRunning = true;

    // Main loop to accept client connections
    while (isRunning) {
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            std::cerr << "Error: Could not accept client connection" << std::endl;
            continue;  // Try again to accept the next connection
        }

        std::cout << "Accepted a client connection!" << std::endl;

        // Receive the HTTP request
        char buffer[4096];
        std::string rawRequest;
        int bytesRead;
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            rawRequest.append(buffer, bytesRead);
            if (rawRequest.find("\r\n\r\n") != std::string::npos) {
                break;
            }
        }

        // Parse the request
        Request request(rawRequest);
        Response response;
        response.setStatusCode(200);  // Default to OK

        // Determine file path and send the file as response
        std::string filePath = serverRoot + request.getUri();
        std::cout << "Trying: " << filePath;
        if (!response.sendFile(filePath)) {
            response.setStatusCode(404);
            response.setBody("404 Not Found");
        }

        // Send the response
        std::string responseStr = response.toString();
        send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
        close(clientSocket);
    }
}

#endif
