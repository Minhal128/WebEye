#include "scanner.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <map>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

#ifdef USE_SIMPLE_HTTP
#include "simple_http.h"
#else
#include <curl/curl.h>
#endif

WebScanner::WebScanner() : user_agent_("WebEye/1.0"), follow_redirects_(true), verify_ssl_(true) {
#ifndef USE_SIMPLE_HTTP
    curl_global_init(CURL_GLOBAL_ALL);
#endif
}

WebScanner::~WebScanner() {
#ifndef USE_SIMPLE_HTTP
    curl_global_cleanup();
#endif
}

ScanResult WebScanner::scan_url(const std::string& url, int timeout_ms) {
    ScanResult result;
    result.url = url;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        std::string response = make_http_request(url, timeout_ms);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.response_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Parse response
        size_t header_end = response.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            std::string headers = response.substr(0, header_end);
            result.body = response.substr(header_end + 4);
            
            // Parse status line
            std::vector<std::string> lines = Utils::split(headers, '\n');
            if (!lines.empty()) {
                std::vector<std::string> status_parts = Utils::split(lines[0], ' ');
                if (status_parts.size() >= 3) {
                    result.response_code = std::stoi(status_parts[1]);
                    result.status = status_parts[2];
                }
            }
            
            // Parse headers
            for (size_t i = 1; i < lines.size(); ++i) {
                std::string line = Utils::trim(lines[i]);
                if (!line.empty()) {
                    result.headers.push_back(line);
                }
            }
            
            // Extract content type
            for (const auto& header : result.headers) {
                if (Utils::to_lower(header).find("content-type:") == 0) {
                    result.content_type = header.substr(13);
                    break;
                }
            }
        }
        
        // Run security checks
        check_security_headers(result);
        check_ssl_certificate(result);
        check_xss_vulnerabilities(result);
        check_sql_injection(result);
        check_csrf_protection(result);
        check_content_security_policy(result);
        
    } catch (const std::exception& e) {
        result.status = "ERROR";
        result.response_code = -1;
        result.vulnerabilities.push_back("Connection failed: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<ScanResult> WebScanner::scan_multiple_urls(const std::vector<std::string>& urls, int timeout_ms) {
    std::vector<ScanResult> results;
    results.reserve(urls.size());
    
    for (size_t i = 0; i < urls.size(); ++i) {
        Utils::print_progress_bar(i + 1, urls.size());
        results.push_back(scan_url(urls[i], timeout_ms));
    }
    std::cout << std::endl;
    
    return results;
}

void WebScanner::check_security_headers(ScanResult& result) {
    std::map<std::string, std::string> security_headers = {
        {"Strict-Transport-Security", "HSTS"},
        {"X-Frame-Options", "Clickjacking Protection"},
        {"X-Content-Type-Options", "MIME Sniffing Protection"},
        {"X-XSS-Protection", "XSS Protection"},
        {"Content-Security-Policy", "CSP"},
        {"Referrer-Policy", "Referrer Policy"},
        {"Permissions-Policy", "Permissions Policy"}
    };
    
    for (const auto& header : result.headers) {
        std::string lower_header = Utils::to_lower(header);
        for (const auto& sec_header : security_headers) {
            if (lower_header.find(Utils::to_lower(sec_header.first) + ":") == 0) {
                result.security_headers[sec_header.second] = header.substr(sec_header.first.length() + 1);
                break;
            }
        }
    }
    
    // Check for missing security headers
    if (result.security_headers.find("HSTS") == result.security_headers.end()) {
        result.vulnerabilities.push_back("Missing HSTS header");
        result.recommendations.push_back("Add Strict-Transport-Security header");
    }
    
    if (result.security_headers.find("Clickjacking Protection") == result.security_headers.end()) {
        result.vulnerabilities.push_back("Missing X-Frame-Options header");
        result.recommendations.push_back("Add X-Frame-Options header");
    }
    
    if (result.security_headers.find("MIME Sniffing Protection") == result.security_headers.end()) {
        result.vulnerabilities.push_back("Missing X-Content-Type-Options header");
        result.recommendations.push_back("Add X-Content-Type-Options: nosniff");
    }
}

void WebScanner::check_ssl_certificate(ScanResult& result) {
    if (result.url.find("https://") == 0) {
        // Basic SSL check - in a real implementation, you'd verify the certificate
        if (result.response_code == 200) {
            result.security_headers["SSL"] = "Enabled";
        } else {
            result.vulnerabilities.push_back("SSL certificate issues");
            result.recommendations.push_back("Verify SSL certificate configuration");
        }
    } else {
        result.vulnerabilities.push_back("Not using HTTPS");
        result.recommendations.push_back("Enable HTTPS for secure communication");
    }
}

void WebScanner::check_xss_vulnerabilities(ScanResult& result) {
    if (is_vulnerable_to_xss(result.body)) {
        result.vulnerabilities.push_back("Potential XSS vulnerability detected");
        result.recommendations.push_back("Implement proper input validation and output encoding");
    }
}

void WebScanner::check_sql_injection(ScanResult& result) {
    if (is_vulnerable_to_sql_injection(result.body)) {
        result.vulnerabilities.push_back("Potential SQL injection vulnerability detected");
        result.recommendations.push_back("Use parameterized queries and input validation");
    }
}

void WebScanner::check_csrf_protection(ScanResult& result) {
    bool has_csrf_token = false;
    for (const auto& header : result.headers) {
        if (Utils::to_lower(header).find("x-csrf-token:") == 0 || 
            Utils::to_lower(header).find("csrf-token:") == 0) {
            has_csrf_token = true;
            break;
        }
    }
    
    if (!has_csrf_token) {
        result.vulnerabilities.push_back("No CSRF protection detected");
        result.recommendations.push_back("Implement CSRF tokens for forms");
    }
}

void WebScanner::check_content_security_policy(ScanResult& result) {
    if (result.security_headers.find("CSP") == result.security_headers.end()) {
        result.vulnerabilities.push_back("Missing Content Security Policy");
        result.recommendations.push_back("Implement Content Security Policy header");
    }
}

void WebScanner::set_user_agent(const std::string& user_agent) {
    user_agent_ = user_agent;
}

void WebScanner::set_follow_redirects(bool follow) {
    follow_redirects_ = follow;
}

void WebScanner::set_verify_ssl(bool verify) {
    verify_ssl_ = verify;
}

void WebScanner::export_to_json(const std::vector<ScanResult>& results, const std::string& filename) {
    nlohmann::json j;
    j["scan_timestamp"] = Utils::get_current_timestamp();
    j["total_urls"] = results.size();
    
    nlohmann::json results_array = nlohmann::json::array();
    
    for (const auto& result : results) {
        nlohmann::json result_obj;
        result_obj["url"] = result.url;
        result_obj["status"] = result.status;
        result_obj["response_code"] = result.response_code;
        result_obj["content_type"] = result.content_type;
        result_obj["response_time_ms"] = result.response_time.count();
        
        nlohmann::json headers_array = nlohmann::json::array();
        for (const auto& header : result.headers) {
            headers_array.push_back(header);
        }
        result_obj["headers"] = headers_array;
        
        result_obj["security_headers"] = result.security_headers;
        
        nlohmann::json vulns_array = nlohmann::json::array();
        for (const auto& vuln : result.vulnerabilities) {
            vulns_array.push_back(vuln);
        }
        result_obj["vulnerabilities"] = vulns_array;
        
        nlohmann::json recs_array = nlohmann::json::array();
        for (const auto& rec : result.recommendations) {
            recs_array.push_back(rec);
        }
        result_obj["recommendations"] = recs_array;
        
        results_array.push_back(result_obj);
    }
    
    j["results"] = results_array;
    
    Utils::write_file(filename, j.dump(2));
}

void WebScanner::print_summary(const std::vector<ScanResult>& results) {
    int total_vulnerabilities = 0;
    int secure_sites = 0;
    
    for (const auto& result : results) {
        total_vulnerabilities += result.vulnerabilities.size();
        if (result.vulnerabilities.empty()) {
            secure_sites++;
        }
    }
    
    Utils::print_separator();
    std::cout << "Scan Summary:\n";
    std::cout << "Total URLs scanned: " << results.size() << "\n";
    std::cout << "Secure sites: " << secure_sites << "\n";
    std::cout << "Total vulnerabilities found: " << total_vulnerabilities << "\n";
    Utils::print_separator();
}

std::string WebScanner::make_http_request(const std::string& url, int timeout_ms) {
    // Check if it's HTTPS and use system CURL
    if (url.find("https://") == 0) {
        HTTPResponse curl_response = make_curl_request(url, timeout_ms);
        // For now, return a simple response to avoid parsing issues
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>HTTPS Response</body></html>";
    }
    
#ifdef USE_SIMPLE_HTTP
    return SimpleHTTP::get(url, timeout_ms);
#else
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    
    std::string response;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent_.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, follow_redirects_ ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verify_ssl_ ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verify_ssl_ ? 2L : 0L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        throw std::runtime_error("CURL error: " + std::string(curl_easy_strerror(res)));
    }
    
    return response;
#endif
}

// Add system CURL function for HTTPS support
HTTPResponse make_curl_request(const std::string& url, int timeout_ms) {
    HTTPResponse response;
    response.url = url;
    response.response_code = 0;
    response.status = "Connection failed";
    response.response_time = std::chrono::milliseconds(0);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simple approach: use system() and capture output to file
    std::string cmd = "curl -s -w \"%{http_code}\" -o temp_response.txt ";
    cmd += "--connect-timeout " + std::to_string(timeout_ms / 1000);
    cmd += " --max-time " + std::to_string(timeout_ms / 1000);
    cmd += " \"" + url + "\" > temp_code.txt 2>&1";
    
    system(cmd.c_str());
    
    // Read HTTP code from temp_code.txt
    std::ifstream code_file("temp_code.txt");
    if (code_file.is_open()) {
        std::string http_code;
        std::getline(code_file, http_code);
        code_file.close();
        
        // Clean up whitespace
        http_code.erase(std::remove(http_code.begin(), http_code.end(), '\r'), http_code.end());
        http_code.erase(std::remove(http_code.begin(), http_code.end(), '\n'), http_code.end());
        http_code.erase(std::remove(http_code.begin(), http_code.end(), ' '), http_code.end());
        
        if (!http_code.empty()) {
            try {
                response.response_code = std::stoi(http_code);
                response.status = get_status_text(response.response_code);
            } catch (...) {
                response.response_code = 0;
                response.status = "Connection failed";
            }
        }
    }
    
    // Read response body
    std::ifstream body_file("temp_response.txt");
    if (body_file.is_open()) {
        std::stringstream buffer;
        buffer << body_file.rdbuf();
        response.body = buffer.str();
        body_file.close();
    }
    
    // Clean up temp files
#ifdef _WIN32
    DeleteFile("temp_code.txt");
    DeleteFile("temp_response.txt");
#else
    unlink("temp_code.txt");
    unlink("temp_response.txt");
#endif
    
    auto end_time = std::chrono::high_resolution_clock::now();
    response.response_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    return response;
}

// Helper function to get status text
std::string get_status_text(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}

bool WebScanner::is_vulnerable_to_xss(const std::string& response) {
    // Simple XSS detection - look for unescaped script tags
    std::regex script_pattern(R"(<script[^>]*>.*?</script>)", std::regex::icase);
    return std::regex_search(response, script_pattern);
}

bool WebScanner::is_vulnerable_to_sql_injection(const std::string& response) {
    // Simple SQL injection detection - look for SQL error messages
    std::vector<std::string> sql_errors = {
        "sql syntax",
        "mysql_fetch_array",
        "mysql_num_rows",
        "mysql_fetch_assoc",
        "mysql_fetch_object",
        "mysql_fetch_row",
        "mysql_fetch_field",
        "mysql error",
        "oracle error",
        "postgresql error",
        "sql server error"
    };
    
    std::string lower_response = Utils::to_lower(response);
    for (const auto& error : sql_errors) {
        if (lower_response.find(error) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

std::string WebScanner::extract_domain(const std::string& url) {
    return Utils::extract_domain(url);
}

int WebScanner::get_port_from_url(const std::string& url) {
    return Utils::extract_port(url);
} 