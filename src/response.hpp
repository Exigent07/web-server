#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>

/**
 * @brief Represents an HTTP response.
 * 
 * The Response class is responsible for constructing and formatting HTTP responses 
 * to be sent to the client.
 */
class Response {
private:
    int statusCode;                                    /** HTTP status code (e.g., 200, 404) */
    std::string statusMessage;                        /** Status message (e.g., "OK", "Not Found") */
    std::unordered_map<std::string, std::string> headers; /** HTTP response headers */
    std::string body;                                 /** Response body content */

    /**
     * @brief Maps all HTTP status codes to their corresponding messages.
     * 
     * @param code HTTP status code.
     * @return Corresponding status message as a string.
     */
    std::string getStatusMessage(int code) const;

public:
    /**
     * @brief Constructs a Response object with default values.
     */
    Response();

    /**
     * @brief Destructor for the Response object.
     */
    ~Response();

    /**
     * @brief Sets the HTTP status code and corresponding message.
     * 
     * @param code HTTP status code.
     */
    void setStatusCode(int code);

    /**
     * @brief Adds a header to the response.
     * 
     * @param key Header name.
     * @param value Header value.
     */
    void addHeader(const std::string& key, const std::string& value);

    /**
     * @brief Sets the body of the response.
     * 
     * @param responseBody Response body content as a string.
     */
    void setBody(const std::string& responseBody);

    /**
     * @brief Converts the response into a properly formatted HTTP response string.
     * 
     * @return HTTP response as a string.
     */
    std::string toString() const;

    /**
     * @brief Sends a file as the response body.
     * 
     * Reads the content of the specified file and sets it as the response body.
     * Also determines the Content-Type header based on the file extension.
     * 
     * @param filePath Path to the file to be sent.
     * @return True if the file was successfully loaded; otherwise, false.
     */
    bool sendFile(const std::string& filePath);
};

// Implementation

Response::Response() : statusCode(200), statusMessage("OK"), body("") {}

Response::~Response() {}

std::string Response::getStatusMessage(int code) const {
    switch (code) {
        // Informational (1xx)
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 102: return "Processing";
        case 103: return "Early Hints";

        // Successful (2xx)
        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 203: return "Non-Authoritative Information";
        case 204: return "No Content";
        case 205: return "Reset Content";
        case 206: return "Partial Content";
        case 207: return "Multi-Status";
        case 208: return "Already Reported";
        case 226: return "IM Used";

        // Redirection (3xx)
        case 300: return "Multiple Choices";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 303: return "See Other";
        case 304: return "Not Modified";
        case 305: return "Use Proxy";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";

        // Client Errors (4xx)
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 402: return "Payment Required";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 406: return "Not Acceptable";
        case 407: return "Proxy Authentication Required";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 410: return "Gone";
        case 411: return "Length Required";
        case 412: return "Precondition Failed";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 416: return "Range Not Satisfiable";
        case 417: return "Expectation Failed";
        case 418: return "I'm a teapot";
        case 421: return "Misdirected Request";
        case 422: return "Unprocessable Entity";
        case 423: return "Locked";
        case 424: return "Failed Dependency";
        case 425: return "Too Early";
        case 426: return "Upgrade Required";
        case 428: return "Precondition Required";
        case 429: return "Too Many Requests";
        case 431: return "Request Header Fields Too Large";
        case 451: return "Unavailable For Legal Reasons";

        // Server Errors (5xx)
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        case 506: return "Variant Also Negotiates";
        case 507: return "Insufficient Storage";
        case 508: return "Loop Detected";
        case 510: return "Not Extended";
        case 511: return "Network Authentication Required";

        default: return "Unknown Status";
    }
}

void Response::setStatusCode(int code) {
    statusCode = code;
    statusMessage = getStatusMessage(code);
}

void Response::addHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void Response::setBody(const std::string& responseBody) {
    body = responseBody;
}

bool Response::sendFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        setStatusCode(404); // File not found
        setBody("404 Not Found");
        return false;
    }

    // Read file content into a string
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();

    setBody(buffer.str());

    // Set Content-Type based on file extension
    std::string contentType = "application/octet-stream"; // Default binary content
    if (filePath.ends_with(".html") || filePath.ends_with(".htm")) contentType = "text/html";
    else if (filePath.ends_with(".css")) contentType = "text/css";
    else if (filePath.ends_with(".js")) contentType = "text/plain";
    else if (filePath.ends_with(".png")) contentType = "image/png";
    else if (filePath.ends_with(".jpg") || filePath.ends_with(".jpeg")) contentType = "image/jpeg";
    else if (filePath.ends_with(".gif")) contentType = "image/gif";
    else if (filePath.ends_with(".txt")) contentType = "text/plain";

    addHeader("Content-Type", contentType);
    addHeader("Content-Length", std::to_string(body.size()));

    return true;
}

std::string Response::toString() const {
    std::ostringstream responseStream;

    // Status line
    responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";

    // Headers
    for (const auto& header : headers) {
        responseStream << header.first << ": " << header.second << "\r\n";
    }

    // Separator between headers and body
    responseStream << "\r\n";

    // Body
    responseStream << body;

    return responseStream.str();
}

#endif