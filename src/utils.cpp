#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <regex>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

namespace Utils {

void print_banner() {
    std::cout << "\n";
    
    // Kali Linux style banner with clear WEB EYE text
    std::cout << Colors::RED << Colors::BOLD;
    std::cout << "  ================================================================\n";
    std::cout << Colors::CYAN << Colors::BOLD;
    std::cout << "\n";
    std::cout << "   W   W  EEEEE  BBBB      EEEEE  Y   Y  EEEEE\n";
    std::cout << "   W   W  E      B   B     E       Y Y   E    \n";
    std::cout << "   W W W  EEEE   BBBB      EEEE     Y    EEEE \n";
    std::cout << "   WW WW  E      B   B     E        Y    E    \n";
    std::cout << "   W   W  EEEEE  BBBB      EEEEE    Y    EEEEE\n";
    std::cout << "\n";
    std::cout << Colors::RED << Colors::BOLD;
    std::cout << "  ================================================================" << Colors::RESET << "\n";
    
    // Tool information with Kali styling
    std::cout << "\n";
    std::cout << Colors::GREEN << Colors::BOLD << "  [+] Web Security Scanner " << Colors::YELLOW << "v1.0.0" << Colors::RESET << "\n";
    std::cout << Colors::WHITE << "  [+] Professional Cybersecurity Assessment Tool" << Colors::RESET << "\n";
    std::cout << Colors::MAGENTA << "  [+] Developed for Penetration Testing" << Colors::RESET << "\n";
    std::cout << "\n";
}

void print_banner_alt() {
    std::cout << Colors::CYAN << Colors::BOLD;
    std::cout << "\n";
    std::cout << "    ██     ██ ███████ ██████      ███████ ██    ██ ███████ \n";
    std::cout << "    ██     ██ ██      ██   ██     ██       ██  ██  ██      \n";
    std::cout << "    ██  █  ██ █████   ██████      █████     ████   █████   \n";
    std::cout << "    ██ ███ ██ ██      ██   ██     ██         ██    ██      \n";
    std::cout << "     ███ ███  ███████ ██████      ███████    ██    ███████ \n";
    std::cout << "                                                           \n";
    std::cout << "  ╔══════════════════════════════════════════════════════╗\n";
    std::cout << "  ║          " << Colors::YELLOW << "Web Security Scanner v1.0.0" << Colors::CYAN << "           ║\n";
    std::cout << "  ║        " << Colors::WHITE << "Comprehensive Cybersecurity CLI Tool" << Colors::CYAN << "      ║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << Colors::RESET;
}

void print_banner_classic() {
    std::cout << Colors::CYAN << Colors::BOLD;
    std::cout << "\n";
    std::cout << " __    __  _____ ______       _____ __    __ _____\n";
    std::cout << "|  |  |  ||  ___|  __  |     |  ___||  |  |  ||  ___|\n";
    std::cout << "|  |__|  || |__ | |__| |     | |__  |  |__|  || |__ \n";
    std::cout << "|   __   ||  __||  __  |     |  __| |   __   ||  __|\n";
    std::cout << "|  |  |  || |___| |  | |     | |___ |  |  |  || |___\n";
    std::cout << "|__|  |__||_____|__|  |_|     |_____|__|  |__||_____|\n";
    std::cout << "                                                    \n";
    std::cout << "              " << Colors::YELLOW << "Web Security Scanner v1.0.0" << Colors::RESET << "\n";
    std::cout << "        " << Colors::WHITE << "Comprehensive Cybersecurity CLI Tool" << Colors::RESET << "\n";
    std::cout << "\n";
    std::cout << Colors::RESET;
}

void print_colored(const std::string& text, const std::string& color) {
    std::cout << color << text << Colors::RESET;
}

void print_progress_bar(int current, int total, int width) {
    float progress = (float)current / total;
    int pos = width * progress;
    
    std::cout << "\r[";
    for (int i = 0; i < width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "% (" << current << "/" << total << ")";
    std::cout.flush();
}

void print_separator(char ch, int width) {
    std::cout << std::string(width, ch) << std::endl;
}

bool is_valid_url(const std::string& url) {
    std::regex url_pattern(R"((https?|ftp)://[^\s/$.?#].[^\s]*)");
    return std::regex_match(url, url_pattern);
}

std::string normalize_url(const std::string& url) {
    std::string normalized = url;
    if (normalized.find("://") == std::string::npos) {
        normalized = "http://" + normalized;
    }
    return normalized;
}

std::string extract_domain(const std::string& url) {
    std::string normalized = normalize_url(url);
    size_t protocol_end = normalized.find("://");
    if (protocol_end == std::string::npos) return "";
    
    size_t domain_start = protocol_end + 3;
    size_t domain_end = normalized.find("/", domain_start);
    if (domain_end == std::string::npos) domain_end = normalized.length();
    
    std::string domain = normalized.substr(domain_start, domain_end - domain_start);
    
    // Remove port if present
    size_t port_pos = domain.find(":");
    if (port_pos != std::string::npos) {
        domain = domain.substr(0, port_pos);
    }
    
    return domain;
}

std::string extract_path(const std::string& url) {
    std::string normalized = normalize_url(url);
    size_t protocol_end = normalized.find("://");
    if (protocol_end == std::string::npos) return "/";
    
    size_t domain_start = protocol_end + 3;
    size_t path_start = normalized.find("/", domain_start);
    if (path_start == std::string::npos) return "/";
    
    return normalized.substr(path_start);
}

int extract_port(const std::string& url) {
    std::string normalized = normalize_url(url);
    size_t protocol_end = normalized.find("://");
    if (protocol_end == std::string::npos) return 80;
    
    size_t domain_start = protocol_end + 3;
    size_t domain_end = normalized.find("/", domain_start);
    if (domain_end == std::string::npos) domain_end = normalized.length();
    
    std::string domain = normalized.substr(domain_start, domain_end - domain_start);
    
    size_t port_pos = domain.find(":");
    if (port_pos != std::string::npos) {
        std::string port_str = domain.substr(port_pos + 1);
        return std::stoi(port_str);
    }
    
    // Default ports
    if (normalized.find("https://") == 0) return 443;
    return 80;
}

std::string get_protocol(const std::string& url) {
    if (url.find("https://") == 0) return "https";
    if (url.find("http://") == 0) return "http";
    if (url.find("ftp://") == 0) return "ftp";
    return "http"; // default
}

std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

std::string replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

bool file_exists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return "";
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool write_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << content;
    return true;
}

std::string get_file_extension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string format_duration(std::chrono::milliseconds duration) {
    auto ms = duration.count();
    if (ms < 1000) return std::to_string(ms) + "ms";
    if (ms < 60000) return std::to_string(ms / 1000) + "s";
    return std::to_string(ms / 60000) + "m " + std::to_string((ms % 60000) / 1000) + "s";
}

bool is_port_open(const std::string& host, int port, int timeout_ms) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;
    
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host.c_str());
    
    // Set timeout
    DWORD timeout = timeout_ms;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    
    int result = connect(sock, (struct sockaddr*)&server, sizeof(server));
    closesocket(sock);
    WSACleanup();
    
    return result == 0;
#else
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host.c_str());
    
    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    int result = connect(sock, (struct sockaddr*)&server, sizeof(server));
    close(sock);
    
    return result == 0;
#endif
}

std::string resolve_dns(const std::string& domain) {
    // For now, return the domain as-is
    // TODO: Implement proper DNS resolution
    return domain;
}

} // namespace Utils 