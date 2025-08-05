#pragma once

#include <string>

namespace SimpleHTTP {
    std::string get(const std::string& url, int timeout_ms = 10000);
    std::string resolve_hostname(const std::string& hostname);
    int create_socket(const std::string& ip, int port, int timeout_ms);
    void close_socket(int sock);
} 