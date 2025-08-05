# WebEye - Web Security Scanner

A comprehensive cybersecurity CLI tool for scanning web applications and identifying security vulnerabilities.

![WebEye Banner](https://img.shields.io/badge/WebEye-Security%20Scanner-blue)
![C++](https://img.shields.io/badge/C++-17-orange)
![License](https://img.shields.io/badge/License-MIT-green)

## Features

- **Security Header Analysis**: Checks for missing security headers like HSTS, CSP, X-Frame-Options
- **SSL/TLS Certificate Verification**: Validates SSL certificates and HTTPS implementation
- **Vulnerability Detection**: Identifies potential XSS, SQL injection, and CSRF vulnerabilities
- **Port Scanning**: Checks for open ports and common vulnerabilities
- **JSON Export**: Exports detailed scan results to JSON format
- **Batch Scanning**: Scan multiple URLs from command line or file
- **Cross-Platform**: Works on Windows, Linux, and macOS
- **Configurable**: Custom timeouts, user agents, and SSL verification options

## Installation

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler
- libcurl (optional, falls back to simple HTTP client)

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/webeye.git
cd webeye

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)

# Install (optional)
sudo make install
```

### Dependencies

#### Windows
```powershell
# Using vcpkg
vcpkg install curl

# Or download from https://curl.se/windows/
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev cmake build-essential
```

#### macOS
```bash
brew install curl cmake
```

## Usage

### Basic Usage

```bash
# Scan a single URL
./webeye https://example.com

# Scan multiple URLs
./webeye https://example.com https://test.com

# Export results to JSON
./webeye -o results.json https://example.com
```

### Advanced Usage

```bash
# Read URLs from file
./webeye -f urls.txt -o scan_results.json

# Custom timeout and user agent
./webeye -t 5000 -u "CustomBot/1.0" https://example.com

# Disable SSL verification (for testing)
./webeye --no-ssl-verify https://example.com

# Verbose output
./webeye -v https://example.com

# Don't follow redirects
./webeye --no-redirects https://example.com
```

### Command Line Options

| Option | Description |
|--------|-------------|
| `-h, --help` | Show help message |
| `-o, --output <file>` | Output results to JSON file |
| `-t, --timeout <ms>` | Request timeout in milliseconds (default: 10000) |
| `-u, --user-agent <ua>` | Custom User-Agent string |
| `-f, --file <file>` | Read URLs from file (one per line) |
| `--no-ssl-verify` | Disable SSL certificate verification |
| `--no-redirects` | Don't follow HTTP redirects |
| `-v, --verbose` | Verbose output |

### Input File Format

Create a text file with one URL per line:

```text
https://example.com
https://test.com
https://demo.org
# Comments are ignored
https://another-site.com
```

## Security Checks

WebEye performs the following security checks:

### Security Headers
- **HSTS (HTTP Strict Transport Security)**: Ensures HTTPS-only communication
- **CSP (Content Security Policy)**: Prevents XSS and other injection attacks
- **X-Frame-Options**: Protects against clickjacking
- **X-Content-Type-Options**: Prevents MIME type sniffing
- **X-XSS-Protection**: Additional XSS protection
- **Referrer-Policy**: Controls referrer information
- **Permissions-Policy**: Controls browser features

### Vulnerability Detection
- **XSS (Cross-Site Scripting)**: Detects unescaped script tags
- **SQL Injection**: Identifies SQL error messages in responses
- **CSRF Protection**: Checks for CSRF tokens
- **SSL/TLS Issues**: Validates certificate configuration

### Network Analysis
- **Port Scanning**: Checks for open ports
- **DNS Resolution**: Validates domain resolution
- **Response Time**: Measures server performance

## Output Format

### Console Output
```
[1/2] https://example.com
  Status: OK
  Response Time: 245ms
  Security Headers:
    ✓ HSTS: max-age=31536000; includeSubDomains
    ✓ CSP: default-src 'self'
  Vulnerabilities:
    ✗ Missing X-Frame-Options header
  Recommendations:
    → Add X-Frame-Options header
```

### JSON Output
```json
{
  "scan_timestamp": "2024-01-15 14:30:25.123",
  "total_urls": 2,
  "results": [
    {
      "url": "https://example.com",
      "status": "OK",
      "response_code": 200,
      "content_type": "text/html",
      "response_time_ms": 245,
      "headers": ["HTTP/1.1 200 OK", "Server: nginx/1.18.0"],
      "security_headers": {
        "HSTS": "max-age=31536000; includeSubDomains",
        "CSP": "default-src 'self'"
      },
      "vulnerabilities": ["Missing X-Frame-Options header"],
      "recommendations": ["Add X-Frame-Options header"]
    }
  ]
}
```

## Examples

### Basic Security Scan
```bash
./webeye https://example.com
```

### Comprehensive Scan with Export
```bash
./webeye -v -o security_report.json -t 15000 https://example.com
```

### Batch Scan from File
```bash
# Create urls.txt with target URLs
echo "https://example.com" > urls.txt
echo "https://test.com" >> urls.txt

# Run batch scan
./webeye -f urls.txt -o batch_results.json
```

### Custom Configuration
```bash
./webeye \
  --user-agent "SecurityBot/2.0" \
  --timeout 8000 \
  --no-ssl-verify \
  --no-redirects \
  -o custom_scan.json \
  https://example.com
```

## Security Considerations

⚠️ **Important**: This tool is for educational and authorized security testing purposes only.

- Only scan websites you own or have explicit permission to test
- Respect robots.txt and rate limiting
- Be aware of legal implications in your jurisdiction
- Use responsibly and ethically

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built with modern C++17
- Uses nlohmann/json for JSON handling
- libcurl for HTTP requests (optional)
- Cross-platform socket implementation

## Support

For issues, questions, or contributions:
- Open an issue on GitHub
- Check the documentation
- Review existing issues and discussions

---

**WebEye** - Making web security scanning accessible and comprehensive. 