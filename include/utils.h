#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <chrono>

namespace Utils {
    // Banner and display functions
    void print_banner();
    void print_banner_alt();
    void print_banner_classic();
    void print_colored(const std::string& text, const std::string& color);
    void print_progress_bar(int current, int total, int width = 50);
    void print_separator(char ch = '=', int width = 80);
    
    // URL utilities
    bool is_valid_url(const std::string& url);
    std::string normalize_url(const std::string& url);
    std::string extract_domain(const std::string& url);
    std::string extract_path(const std::string& url);
    int extract_port(const std::string& url);
    std::string get_protocol(const std::string& url);
    
    // String utilities
    std::string to_lower(const std::string& str);
    std::string to_upper(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
    bool contains(const std::string& haystack, const std::string& needle);
    std::string replace(const std::string& str, const std::string& from, const std::string& to);
    
    // File utilities
    bool file_exists(const std::string& filename);
    std::string read_file(const std::string& filename);
    bool write_file(const std::string& filename, const std::string& content);
    std::string get_file_extension(const std::string& filename);
    
    // Time utilities
    std::string get_current_timestamp();
    std::string format_duration(std::chrono::milliseconds duration);
    
    // Network utilities
    bool is_port_open(const std::string& host, int port, int timeout_ms = 5000);
    std::string resolve_dns(const std::string& domain);
    
    // Color codes for terminal output
    namespace Colors {
        const std::string RESET = "\033[0m";
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string BLUE = "\033[34m";
        const std::string MAGENTA = "\033[35m";
        const std::string CYAN = "\033[36m";
        const std::string WHITE = "\033[37m";
        const std::string GRAY = "\033[90m";
        const std::string BOLD = "\033[1m";
        const std::string UNDERLINE = "\033[4m";
    }
} 