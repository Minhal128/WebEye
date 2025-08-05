#include "scanner.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream> // Added for file reading

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
        
        std::vector<std::string> urls;
        std::string output_file;
        int timeout_ms = 10000;
        std::string user_agent = "WebEye/1.0";
        bool verify_ssl = true;
        bool follow_redirects = true;
        bool verbose = false;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "-h" || arg == "--help") {
                print_usage();
                return 0;
            } else if (arg == "-o" || arg == "--output") {
                if (i + 1 < argc) {
                    output_file = argv[++i];
                } else {
                    std::cerr << "Error: Missing output filename\n";
                    return 1;
                }
            } else if (arg == "-t" || arg == "--timeout") {
                if (i + 1 < argc) {
                    timeout_ms = std::stoi(argv[++i]);
                } else {
                    std::cerr << "Error: Missing timeout value\n";
                    return 1;
                }
            } else if (arg == "-u" || arg == "--user-agent") {
                if (i + 1 < argc) {
                    user_agent = argv[++i];
                } else {
                    std::cerr << "Error: Missing User-Agent string\n";
                    return 1;
                }
            } else if (arg == "-f" || arg == "--file") {
                if (i + 1 < argc) {
                    try {
                        urls = read_urls_from_file(argv[++i]);
                    } catch (const std::exception& e) {
                        std::cerr << "Error reading file: " << e.what() << "\n";
                        return 1;
                    }
                } else {
                    std::cerr << "Error: Missing filename\n";
                    return 1;
                }
            } else if (arg == "--no-ssl-verify") {
                verify_ssl = false;
            } else if (arg == "--no-redirects") {
                follow_redirects = false;
            } else if (arg == "-v" || arg == "--verbose") {
                verbose = true;
            } else if (arg[0] == '-') {
                std::cerr << "Error: Unknown option " << arg << "\n";
                print_usage();
                return 1;
            } else {
                // This is a URL
                if (!Utils::is_valid_url(arg)) {
                    std::cerr << "Warning: Invalid URL format: " << arg << "\n";
                }
                urls.push_back(arg);
            }
        }
        
        if (urls.empty()) {
            std::cerr << "Error: No URLs specified\n";
            print_usage();
            return 1;
        }
        
        // Configure scanner
        WebScanner scanner;
        scanner.set_user_agent(user_agent);
        scanner.set_follow_redirects(follow_redirects);
        scanner.set_verify_ssl(verify_ssl);
        
        if (verbose) {
            std::cout << "Configuration:\n";
            std::cout << "  Timeout: " << timeout_ms << "ms\n";
            std::cout << "  User-Agent: " << user_agent << "\n";
            std::cout << "  SSL Verification: " << (verify_ssl ? "Enabled" : "Disabled") << "\n";
            std::cout << "  Follow Redirects: " << (follow_redirects ? "Enabled" : "Disabled") << "\n";
            std::cout << "  URLs to scan: " << urls.size() << "\n";
            Utils::print_separator();
        }
        
        // Start scanning
        std::cout << Utils::Colors::CYAN << "Starting security scan...\n" << Utils::Colors::RESET;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<ScanResult> results = scanner.scan_multiple_urls(urls, timeout_ms);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Print results
        std::cout << "\n";
        Utils::print_separator();
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            
            std::cout << Utils::Colors::BOLD << "[" << (i + 1) << "/" << results.size() << "] " 
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
        
        // Print summary
        scanner.print_summary(results);
        
        std::cout << "Total scan time: " << Utils::format_duration(total_time) << "\n";
        
        // Export to JSON if requested
        if (!output_file.empty()) {
            try {
                scanner.export_to_json(results, output_file);
                std::cout << Utils::Colors::GREEN << "Results exported to: " << output_file << Utils::Colors::RESET << "\n";
            } catch (const std::exception& e) {
                std::cerr << Utils::Colors::RED << "Error exporting results: " << e.what() << Utils::Colors::RESET << "\n";
                return 1;
            }
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