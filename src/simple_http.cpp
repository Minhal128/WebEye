#include "simple_http.h"
#include <iostream>
#include <sstream>
#include <regex>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

namespace SimpleHTTP {

std::string get(const std::string& url, int timeout_ms) {
    // Parse URL
    std::string protocol, host, path;
    int port;
    
    if (url.find("https://") == 0) {
        protocol = "https";
        host = url.substr(8);
        port = 443;
    } else if (url.find("http://") == 0) {
        protocol = "http";
        host = url.substr(7);
        port = 80;
    } else {
        protocol = "http";
        host = url;
        port = 80;
    }
    
    size_t path_pos = host.find('/');
    if (path_pos != std::string::npos) {
        path = host.substr(path_pos);
        host = host.substr(0, path_pos);
    } else {
        path = "/";
    }
    
    // Check for custom port
    size_t port_pos = host.find(':');
    if (port_pos != std::string::npos) {
        port = std::stoi(host.substr(port_pos + 1));
        host = host.substr(0, port_pos);
    }
    
    // Resolve hostname
    std::string ip = resolve_hostname(host);
    if (ip.empty()) {
        throw std::runtime_error("Failed to resolve hostname: " + host);
    }
    
    // Create socket and connect
    int sock = create_socket(ip, port, timeout_ms);
    if (sock < 0) {
        throw std::runtime_error("Failed to connect to " + host + ":" + std::to_string(port));
    }
    
    // Send HTTP request
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: WebEye/1.0\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";
    
    if (send(sock, request.c_str(), request.length(), 0) < 0) {
        close_socket(sock);
        throw std::runtime_error("Failed to send HTTP request");
    }
    
    // Receive response
    std::string response;
    char buffer[4096];
    int bytes_received;
    
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        response += buffer;
    }
    
    close_socket(sock);
    return response;
}

std::string resolve_hostname(const std::string& hostname) {
#ifdef _WIN32
    struct hostent* he = gethostbyname(hostname.c_str());
    if (he == NULL) {
        return "";
    }
    
    struct in_addr addr;
    addr.s_addr = *(unsigned long*)he->h_addr_list[0];
    return inet_ntoa(addr);
#else
    struct hostent* he = gethostbyname(hostname.c_str());
    if (he == NULL) {
        return "";
    }
    
    struct in_addr addr;
    addr.s_addr = *(unsigned long*)he->h_addr_list[0];
    return inet_ntoa(addr);
#endif
}

int create_socket(const std::string& ip, int port, int timeout_ms) {
#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        return -1;
    }
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    
    // Set timeout
    DWORD timeout = timeout_ms;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) != 0) {
        closesocket(sock);
        return -1;
    }
    
    return (int)sock;
#else
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    
    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) != 0) {
        close(sock);
        return -1;
    }
    
    return sock;
#endif
}

void close_socket(int sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

} // namespace SimpleHTTP 