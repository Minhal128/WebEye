#include "scanner.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream> // Added for file reading
#include <limits> // Added for std::numeric_limits

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

void print_usage() {
    std::cout << "Usage: webeye [OPTIONS] <URL(s)>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -o, --output <file>     Output results to JSON file\n";
    std::cout << "  -t, --timeout <ms>      Request timeout in milliseconds (default: 10000)\n";
    std::cout << "  -u, --user-agent <ua>   Custom User-Agent string\n";
    std::cout << "  -f, --file <file>       Read URLs from file (one per line)\n";
    std::cout << "  --no-ssl-verify         Disable SSL certificate verification\n";
    std::cout << "  --no-redirects          Don't follow HTTP redirects\n";
    std::cout << "  -v, --verbose           Verbose output\n";
    std::cout << "\n";
    std::cout << "Examples:\n";
    std::cout << "  webeye https://example.com\n";
    std::cout << "  webeye -o results.json https://example.com https://test.com\n";
    std::cout << "  webeye -f urls.txt -o scan_results.json\n";
    std::cout << "  webeye -t 5000 --no-ssl-verify https://example.com\n";
}

std::vector<std::string> read_urls_from_file(const std::string& filename) {
    std::vector<std::string> urls;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (!line.empty() && line[0] != '#') {
            urls.push_back(line);
        }
    }
    
    return urls;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error: Failed to initialize Winsock\n";
        return 1;
    }
#endif

    try {
        Utils::print_banner();
        const std::string notice = "Tool by : cybermon & 3ntr0py";
        const int term_width = 80;
        int pad = (term_width - static_cast<int>(notice.size())) / 2;
        if (pad < 0) pad = 0;
        const std::string border(term_width, '=');
        std::cout << "\033[1;31m" << border << "\033[0m\n";
        std::cout << "\033[1;31m"
                  << std::string(pad, ' ') << notice << "\033[0m\n";
        std::cout << "\033[1;31m" << border << "\033[0m\n";
        // std::cout << "\033[1;31mthis tool is made by cybermon \u2764\033[0m\n";

        int timeout_ms = 10000;
        std::string user_agent = "WebEye/1.0";
        bool verify_ssl = true;
        bool follow_redirects = true;
        bool verbose = false;
        std::string last_scanned_url;
        std::vector<ScanResult> last_scan_results;
        WebScanner scanner;
        scanner.set_user_agent(user_agent);
        scanner.set_follow_redirects(follow_redirects);
        scanner.set_verify_ssl(verify_ssl);

        while (true) {
            std::cout << "\033[1;33mMain Menu:\n\033[0m";
            std::cout << "\033[1;37m"
                      << "  1. URL Scan\n"
                      << "  2. Export JSON\n"
                      << "  3. Timeout\n"
                      << "  4. Verbose\n"
                      << "  5. Exit\n"
                      << "\033[0m";
            std::cout << "\n\033[1;32m-> Choose: \033[0m";
            int menu_choice;
            std::cin >> menu_choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (menu_choice == 1) {
                std::cout << "Enter target URL: ";
                std::string url;
                std::getline(std::cin, url);
                if (!Utils::is_valid_url(url)) {
                    std::cerr << "Invalid URL format.\n";
                    continue;
                }
                std::vector<std::string> urls = {url};
                std::cout << Utils::Colors::CYAN << "Starting security scan...\n" << Utils::Colors::RESET;
                auto start_time = std::chrono::high_resolution_clock::now();
                last_scan_results = scanner.scan_multiple_urls(urls, timeout_ms);
                auto end_time = std::chrono::high_resolution_clock::now();
                auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                last_scanned_url = url;
                Utils::print_separator();
                for (size_t i = 0; i < last_scan_results.size(); ++i) {
                    const auto& result = last_scan_results[i];
                    std::cout << Utils::Colors::BOLD << "[" << (i + 1) << "/" << last_scan_results.size() << "] "
                              << result.url << Utils::Colors::RESET << "\n";
                    
                    std::cout << "  Status: ";
                    if (result.response_code == 200) {
                        std::cout << Utils::Colors::GREEN << "[OK] SITE IS WORKING AND ACCESSIBLE" << Utils::Colors::RESET;
                    } else if (result.response_code > 0 && result.response_code < 400) {
                        std::cout << Utils::Colors::GREEN << "[OK] SITE IS WORKING (" << result.response_code << " " << result.status << ")" << Utils::Colors::RESET;
                    } else if (result.response_code >= 400 && result.response_code < 500) {
                        std::cout << Utils::Colors::YELLOW << "[WARN] SITE RESPONDED WITH ERROR (" << result.response_code << " " << result.status << ")" << Utils::Colors::RESET;
                    } else if (result.response_code >= 500) {
                        std::cout << Utils::Colors::RED << "[ERROR] SERVER ERROR (" << result.response_code << " " << result.status << ")" << Utils::Colors::RESET;
                    } else {
                        std::cout << Utils::Colors::RED << "[ERROR] CONNECTION FAILED - SITE NOT ACCESSIBLE" << Utils::Colors::RESET;
                    }
                    std::cout << "\n";
                    
                    // Add site safety explanation
                    std::cout << "  Site Safety: ";
                    if (result.response_code == 200) {
                        std::cout << Utils::Colors::GREEN << "[OK] Site is accessible and responding correctly" << Utils::Colors::RESET << "\n";
                        std::cout << "    " << Utils::Colors::WHITE << "Example: Like Facebook, Google, or any working website" << Utils::Colors::RESET << "\n";
                    } else if (result.response_code > 0 && result.response_code < 400) {
                        std::cout << Utils::Colors::GREEN << "[OK] Site is accessible (may have redirects or other responses)" << Utils::Colors::RESET << "\n";
                        std::cout << "    " << Utils::Colors::WHITE << "Example: Sites that redirect or show different content" << Utils::Colors::RESET << "\n";
                    } else if (result.response_code >= 400 && result.response_code < 500) {
                        std::cout << Utils::Colors::YELLOW << "[WARN] Site responded but with an error (client-side issue)" << Utils::Colors::RESET << "\n";
                        std::cout << "    " << Utils::Colors::WHITE << "Example: 404 Not Found, 403 Forbidden, etc." << Utils::Colors::RESET << "\n";
                    } else if (result.response_code >= 500) {
                        std::cout << Utils::Colors::RED << "[ERROR] Server is having problems (server-side error)" << Utils::Colors::RESET << "\n";
                        std::cout << "    " << Utils::Colors::WHITE << "Example: Server down, maintenance, or technical issues" << Utils::Colors::RESET << "\n";
                    } else {
                        std::cout << Utils::Colors::RED << "[ERROR] Cannot reach the site at all" << Utils::Colors::RESET << "\n";
                        std::cout << "    " << Utils::Colors::WHITE << "Example: Wrong URL, site doesn't exist, or network issues" << Utils::Colors::RESET << "\n";
                    }
                    
                    std::cout << "  Response Time: " << Utils::format_duration(result.response_time) << "\n";
                    
                    if (!result.security_headers.empty()) {
                        std::cout << "  Security Headers:\n";
                        for (const auto& header : result.security_headers) {
                            std::cout << "    " << Utils::Colors::GREEN << "[OK] " << header.first << Utils::Colors::RESET << ": " << header.second << "\n";
                        }
                    }
                    
                    if (!result.vulnerabilities.empty()) {
                        std::cout << "  Vulnerabilities:\n";
                        for (const auto& vuln : result.vulnerabilities) {
                            std::cout << "    " << Utils::Colors::RED << "[X] " << vuln << Utils::Colors::RESET << "\n";
                        }
                    }
                    
                    if (!result.recommendations.empty()) {
                        std::cout << "  Recommendations:\n";
                        for (const auto& rec : result.recommendations) {
                            std::cout << "    " << Utils::Colors::YELLOW << "[TIP] " << rec << Utils::Colors::RESET << "\n";
                        }
                    }
                    
                    std::cout << "\n";
                }
                scanner.print_summary(last_scan_results);
                std::cout << "Total scan time: " << Utils::format_duration(total_time) << "\n";
            } else if (menu_choice == 2) {
                if (last_scan_results.empty()) {
                    std::cout << "No scan data available. Please scan a URL first.\n";
                    continue;
                }
                std::cout << "Exporting report for the most recently scanned URL\n";
                std::string output_file = "scan_report.json";
                try {
                    scanner.export_to_json(last_scan_results, output_file);
                    std::cout << Utils::Colors::GREEN << "Results exported to: " << output_file << Utils::Colors::RESET << "\n";
                } catch (const std::exception& e) {
                    std::cerr << Utils::Colors::RED << "Error exporting results: " << e.what() << Utils::Colors::RESET << "\n";
                }
            } else if (menu_choice == 3) {
                std::cout << "Enter timeout value in seconds: ";
                int timeout_sec;
                std::cin >> timeout_sec;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (timeout_sec <= 0) {
                    std::cerr << "Invalid timeout value.\n";
                    continue;
                }
                timeout_ms = timeout_sec * 1000;
                std::cout << "Timeout set to " << timeout_sec << " seconds.\n";
            } else if (menu_choice == 4) {
                verbose = true;
                std::cout << "Verbose mode activated. Enter target URL: ";
                std::string url;
                std::getline(std::cin, url);
                if (!Utils::is_valid_url(url)) {
                    std::cerr << "Invalid URL format.\n";
                    continue;
                }
                std::vector<std::string> urls = {url};
                std::cout << Utils::Colors::CYAN << "Starting security scan (verbose)...\n" << Utils::Colors::RESET;
                auto start_time = std::chrono::high_resolution_clock::now();
                last_scan_results.clear();
                for (const auto& vurl : urls) {
                    // Real-time log: print before/after each scan
                    std::cout << "Scanning: " << vurl << "...\n";
                    auto vresult = scanner.scan_url(vurl, timeout_ms);
                    // Print detailed info for this scan
                    std::cout << "  Response code: " << vresult.response_code << "\n";
                    std::cout << "  Status: " << vresult.status << "\n";
                    std::cout << "  Response time: " << Utils::format_duration(vresult.response_time) << "\n";
                    if (!vresult.security_headers.empty()) {
                        std::cout << "  Security Headers:\n";
                        for (const auto& header : vresult.security_headers) {
                            std::cout << "    [OK] " << header.first << ": " << header.second << "\n";
                        }
                    }
                    if (!vresult.vulnerabilities.empty()) {
                        std::cout << "  Vulnerabilities:\n";
                        for (const auto& vuln : vresult.vulnerabilities) {
                            std::cout << "    [X] " << vuln << "\n";
                        }
                    }
                    if (!vresult.recommendations.empty()) {
                        std::cout << "  Recommendations:\n";
                        for (const auto& rec : vresult.recommendations) {
                            std::cout << "    [TIP] " << rec << "\n";
                        }
                    }
                    last_scan_results.push_back(vresult);
                }
                auto end_time = std::chrono::high_resolution_clock::now();
                auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                scanner.print_summary(last_scan_results);
                std::cout << "Total scan time: " << Utils::format_duration(total_time) << "\n";
                verbose = false;
            } else if (menu_choice == 5) {
                std::cout << "Exiting...\n";
                break;
            } else {
                std::cerr << "Invalid option. Please try again.\n";
            }
            Utils::print_separator();
        }
        
    } catch (const std::exception& e) {
        std::cerr << Utils::Colors::RED << "Fatal error: " << e.what() << Utils::Colors::RESET << "\n";
        return 1;
    }
    
#ifdef _WIN32
    WSACleanup();
#endif
    
    return 0;
} 