#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>

// JSON library will be included where needed

struct HTTPResponse {
    std::string url;
    int response_code;
    std::string status;
    std::string body;
    std::map<std::string, std::string> headers;
    std::chrono::milliseconds response_time;
};

struct ScanResult {
    std::string url;
    std::string status;
    int response_code;
    std::string content_type;
    std::vector<std::string> headers;
    std::string body;
    std::chrono::milliseconds response_time;
    std::map<std::string, std::string> security_headers;
    std::vector<std::string> vulnerabilities;
    std::vector<std::string> recommendations;
};

class WebScanner {
public:
    WebScanner();
    ~WebScanner();
    
    // Main scanning methods
    ScanResult scan_url(const std::string& url, int timeout_ms = 10000);
    std::vector<ScanResult> scan_multiple_urls(const std::vector<std::string>& urls, int timeout_ms = 10000);
    
    // Security checks
    void check_ssl_certificate(ScanResult& result);
    void check_security_headers(ScanResult& result);
    void check_open_ports(ScanResult& result);
    void check_directory_traversal(ScanResult& result);
    void check_sql_injection(ScanResult& result);
    void check_xss_vulnerabilities(ScanResult& result);
    void check_csrf_protection(ScanResult& result);
    void check_content_security_policy(ScanResult& result);
    
    // Configuration
    void set_user_agent(const std::string& user_agent);
    void set_follow_redirects(bool follow);
    void set_verify_ssl(bool verify);
    
    // Output
    void export_to_json(const std::vector<ScanResult>& results, const std::string& filename);
    void print_summary(const std::vector<ScanResult>& results);
    
private:
    std::string user_agent_;
    bool follow_redirects_;
    bool verify_ssl_;
    
    // Helper methods
    std::string make_http_request(const std::string& url, int timeout_ms);
    std::map<std::string, std::string> parse_headers(const std::string& header_string);
    bool is_vulnerable_to_xss(const std::string& response);
    bool is_vulnerable_to_sql_injection(const std::string& response);
    std::string extract_domain(const std::string& url);
    int get_port_from_url(const std::string& url);
};

// Helper functions for system CURL
HTTPResponse make_curl_request(const std::string& url, int timeout_ms);
std::string get_status_text(int code); 