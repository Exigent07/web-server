#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <unordered_map>
#include <sstream>

/**
 * @enum METHOD
 * @brief Enumeration for HTTP request methods.
 */
enum METHOD {
    GET,      /**< HTTP GET method */
    POST,     /**< HTTP POST method */
    PUT,      /**< HTTP PUT method */
    DELETE,   /**< HTTP DELETE method */
    PATCH,    /**< HTTP PATCH method */
    OPTIONS,  /**< HTTP OPTIONS method */
    HEAD,     /**< HTTP HEAD method */
    TRACE,    /**< HTTP TRACE method */
    CONNECT,  /**< HTTP CONNECT method */
    UNKNOWN   /**< Unknown HTTP method */
};

/**
 * @brief Parses a string to determine the HTTP method.
 * 
 * @param method A string representation of the HTTP method (e.g., "GET", "POST").
 * @return METHOD The corresponding METHOD enum value.
 */
METHOD parseMethod(const std::string& method) {
    if (method == "GET") return GET;
    if (method == "POST") return POST;
    if (method == "PUT") return PUT;
    if (method == "DELETE") return DELETE;
    if (method == "PATCH") return PATCH;
    if (method == "OPTIONS") return OPTIONS;
    if (method == "HEAD") return HEAD;
    if (method == "TRACE") return TRACE;
    if (method == "CONNECT") return CONNECT;
    return UNKNOWN;
}

/**
 * @class Request
 * @brief Represents an HTTP request, providing methods to parse and store its components.
 */
class Request {
private:
    METHOD method;                                  /**< HTTP method of the request */
    std::string uri;                                /**< URI of the request */
    std::unordered_map<std::string, std::string> headers; /**< Headers of the request */
    std::string body;                               /**< Body of the request */

public:
    /**
     * @brief Default constructor.
     */
    Request();

    /**
     * @brief Constructs a Request object and parses a raw HTTP request string.
     * 
     * @param rawRequest The raw HTTP request as a string.
     */
    Request(const std::string& rawRequest);

    /**
     * @brief Destructor.
     */
    ~Request();

    /**
     * @brief Gets the HTTP method of the request.
     * 
     * @return METHOD The HTTP method of the request.
     */
    METHOD getMethod() const { return method; }

    /**
     * @brief Gets the URI of the request.
     * 
     * @return std::string The URI of the request.
     */
    const std::string& getUri() const { return uri; }

    /**
     * @brief Gets the headers of the request.
     * 
     * @return const std::unordered_map<std::string, std::string>& 
     *         A reference to the headers map.
     */
    const std::unordered_map<std::string, std::string>& getHeaders() const { return headers; }

    /**
     * @brief Gets the body of the request.
     * 
     * @return const std::string& The body of the request.
     */
    const std::string& getBody() const { return body; }

    /**
     * @brief Parses a raw HTTP request string into its components.
     * 
     * @param rawRequest The raw HTTP request as a string.
     */
    void parse(const std::string& rawRequest);
};

// Implementation

Request::Request() : method(UNKNOWN), uri(""), body("") {}

Request::Request(const std::string& rawRequest) {
    parse(rawRequest);
}

Request::~Request() {}

void Request::parse(const std::string& rawRequest) {
    std::istringstream stream(rawRequest);
    std::string line;

    // Parse the request line
    if (std::getline(stream, line)) {
        std::istringstream requestLine(line);
        std::string methodStr, uriStr, httpVersion;

        requestLine >> methodStr >> uriStr >> httpVersion;
        method = parseMethod(methodStr);
        uri = uriStr;
    }

    // Parse headers
    while (std::getline(stream, line) && line != "\r") {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string headerKey = line.substr(0, colon);
            std::string headerValue = line.substr(colon + 1);
            // Trim spaces from the header value
            headerValue.erase(0, headerValue.find_first_not_of(" \t"));
            headerValue.erase(headerValue.find_last_not_of(" \t") + 1);
            headers[headerKey] = headerValue;
        }
    }

    // Parse body (remaining lines)
    std::string bodyLine;
    while (std::getline(stream, bodyLine)) {
        body += bodyLine + "\n";
    }
    if (!body.empty()) {
        body.pop_back(); // Remove the last newline character
    }
}

#endif
