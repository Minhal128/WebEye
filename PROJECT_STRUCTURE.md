# WebEye Project Structure

This document provides a detailed overview of the WebEye project structure and organization.

## 📁 Directory Structure

```
webeye/
├── src/                          # Source code directory
│   ├── main.cpp                  # Main entry point and CLI interface
│   ├── scanner.h                 # Scanner class header file
│   ├── scanner.cpp               # Scanner class implementation
│   ├── utils.h                   # Utility functions header
│   └── utils.cpp                 # Utility functions implementation
├── build/                        # Build output directory (created during build)
│   ├── bin/                      # Compiled executables
│   └── obj/                      # Object files (Makefile builds)
├── CMakeLists.txt                # CMake build configuration
├── Makefile                      # Alternative Make-based build system
├── build.sh                      # Linux/macOS build script
├── build.bat                     # Windows build script
├── test.sh                       # Test suite script
├── README.md                     # Main project documentation
├── PROJECT_STRUCTURE.md          # This file
└── LICENSE                       # MIT License
```

## 🔧 Source Code Files

### `src/main.cpp`
**Purpose**: Main entry point and command-line interface
**Key Features**:
- Command-line argument parsing
- Interactive mode implementation
- User interface and menu system
- Error handling and usage display

**Key Functions**:
- `main()`: Entry point with argument parsing
- `printBanner()`: Displays WebEye ASCII art banner
- `printUsage()`: Shows command-line help
- `printInteractiveMenu()`: Interactive mode menu

### `src/scanner.h`
**Purpose**: Scanner class header file
**Key Features**:
- Singleton pattern for Scanner class
- ScanResult structure definition
- Public interface declarations
- Private helper method declarations

**Key Structures**:
```cpp
struct ScanResult {
    std::string url;
    std::string ip;
    std::string server;
    std::string status;
    std::map<std::string, std::string> headers;
    std::vector<std::string> openPorts;
    std::vector<std::string> vulnerabilities;
    std::vector<std::string> technologies;
    std::string sslInfo;
    std::string responseTime;
    std::string timestamp;
};
```

### `src/scanner.cpp`
**Purpose**: Core scanning functionality implementation
**Key Features**:
- Port scanning (common ports: 21, 22, 23, 25, 53, 80, 110, 143, 443, 993, 995, 3306, 3389, 5432, 8080, 8443)
- HTTP header analysis
- Vulnerability detection
- Technology detection
- SSL/TLS certificate validation
- JSON export functionality

**Key Methods**:
- `scan()`: Main scanning orchestration
- `scanPorts()`: Port scanning functionality
- `scanHeaders()`: HTTP header analysis
- `scanVulnerabilities()`: Security vulnerability checks
- `exportResults()`: JSON export functionality
- `detectTechnologies()`: Web technology detection
- `checkVulnerabilities()`: Security assessment

### `src/utils.h` & `src/utils.cpp`
**Purpose**: Utility functions and helper methods
**Key Features**:
- String manipulation functions
- Time and date utilities
- Network utility functions
- File system helpers

**Key Functions**:
- `trim()`: String whitespace trimming
- `toLower()` / `toUpper()`: String case conversion
- `getCurrentTimestamp()`: Current timestamp generation
- `isValidUrl()`: URL validation
- `extractHostname()`: Domain extraction from URLs

## 🏗️ Build System Files

### `CMakeLists.txt`
**Purpose**: CMake build configuration
**Key Features**:
- C++17 standard requirement
- Automatic dependency management
- Cross-platform build support
- Optimized compiler flags

**Dependencies**:
- libcurl (HTTP client library)
- nlohmann/json (JSON parsing)
- pthread (threading support)

### `Makefile`
**Purpose**: Alternative Make-based build system
**Key Features**:
- Simple dependency management
- Debug and release builds
- Installation targets
- Clean targets

### `build.sh`
**Purpose**: Linux/macOS build automation script
**Key Features**:
- Automatic dependency detection and installation
- Multi-distribution support (Ubuntu, CentOS, Arch, macOS)
- Build, test, and install automation
- Colored output and progress indicators

**Supported OS**:
- Ubuntu/Debian (apt-get)
- CentOS/RHEL (yum)
- Arch Linux (pacman)
- macOS (Homebrew)

### `build.bat`
**Purpose**: Windows build automation script
**Key Features**:
- Visual Studio and MinGW support
- Automatic compiler detection
- Windows-specific installation
- PATH environment setup

## 🧪 Testing Files

### `test.sh`
**Purpose**: Comprehensive test suite
**Key Features**:
- Basic functionality tests
- Command-line interface tests
- Network connectivity tests
- JSON export validation
- Interactive mode testing

**Test Categories**:
- Help command functionality
- Invalid argument handling
- Basic scanning operations
- Verbose mode testing
- Timeout functionality
- JSON export format validation
- Interactive mode startup

## 📚 Documentation Files

### `README.md`
**Purpose**: Main project documentation
**Sections**:
- Project overview and features
- Installation instructions
- Usage examples and command-line options
- Scan results explanation
- JSON export format documentation
- Troubleshooting guide
- Development guidelines

### `PROJECT_STRUCTURE.md`
**Purpose**: This file - detailed project structure documentation

### `LICENSE`
**Purpose**: MIT License for the project

## 🔄 Build Process

### Linux/macOS Build Process
1. **Dependency Check**: `build.sh` checks for required packages
2. **Dependency Installation**: Automatically installs missing packages
3. **CMake Configuration**: Generates build files
4. **Compilation**: Builds the executable
5. **Testing**: Runs basic functionality tests
6. **Installation**: Installs to system (optional)

### Windows Build Process
1. **Compiler Detection**: Detects Visual Studio or MinGW
2. **CMake Configuration**: Generates appropriate build files
3. **Compilation**: Builds the executable
4. **Testing**: Runs basic functionality tests
5. **Installation**: Installs to Program Files (optional)

## 🎯 Key Features Implementation

### Port Scanning
- **Location**: `scanner.cpp::isPortOpen()`
- **Method**: TCP socket connection testing
- **Ports**: 21, 22, 23, 25, 53, 80, 110, 143, 443, 993, 995, 3306, 3389, 5432, 8080, 8443
- **Timeout**: 2 seconds per port

### HTTP Header Analysis
- **Location**: `scanner.cpp::getHeaders()`
- **Method**: libcurl HTTP requests
- **Features**: Header parsing, server detection, security header analysis

### Vulnerability Detection
- **Location**: `scanner.cpp::checkVulnerabilities()`
- **Checks**:
  - Missing X-Frame-Options (Clickjacking)
  - Missing X-Content-Type-Options (MIME sniffing)
  - Missing X-XSS-Protection (XSS)
  - Missing HSTS (HTTPS downgrade)
  - Server information disclosure

### Technology Detection
- **Location**: `scanner.cpp::detectTechnologies()`
- **Detections**:
  - Web servers (Apache, Nginx, IIS, Cloudflare)
  - Frameworks (PHP, ASP.NET, Express.js, Django)
  - Security headers (X-Frame-Options, HSTS, etc.)

### JSON Export
- **Location**: `scanner.cpp::exportResults()`
- **Format**: Structured JSON with scan metadata and results
- **Features**: Pretty-printed output, comprehensive scan data

## 🔧 Configuration Options

### Command Line Options
- `--url <target-url>`: Target website (required)
- `--export <output.json>`: JSON export file
- `--timeout <seconds>`: Request timeout (default: 10)
- `--verbose`: Detailed output mode
- `--help`: Show help message
- `--interactive`: Start interactive mode

### Build Configuration
- **C++ Standard**: C++17
- **Optimization**: -O2 (Release), -O0 (Debug)
- **Warnings**: -Wall -Wextra
- **Threading**: pthread support

## 🚀 Deployment

### Linux/macOS Deployment
```bash
./build.sh                    # Full build and install
./build.sh --install-deps     # Install dependencies only
./build.sh --build           # Build only
./build.sh --test            # Run tests only
```

### Windows Deployment
```cmd
build.bat                    # Full build and install
build.bat --check-deps       # Check dependencies
build.bat --build           # Build only
build.bat --test            # Run tests only
```

### Manual Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 🔍 Code Quality

### Coding Standards
- **Language**: C++17
- **Style**: Consistent naming conventions
- **Documentation**: Inline comments for complex logic
- **Error Handling**: Comprehensive error checking
- **Memory Management**: RAII principles

### Dependencies
- **libcurl**: HTTP client functionality
- **nlohmann/json**: JSON parsing and generation
- **Standard Library**: String manipulation, networking, file I/O
- **System Libraries**: Socket programming, DNS resolution

## 📈 Future Enhancements

### Potential Additions
- **Additional Vulnerability Checks**: SQL injection, XSS testing
- **Custom Port Ranges**: User-defined port scanning
- **Plugin System**: Extensible scanning modules
- **Web Interface**: Web-based results viewer
- **Database Integration**: Scan history storage
- **Reporting**: PDF/HTML report generation
- **API Integration**: Third-party security service integration

### Performance Optimizations
- **Parallel Scanning**: Multi-threaded port scanning
- **Connection Pooling**: Reuse HTTP connections
- **Caching**: Cache DNS resolutions and results
- **Compression**: Compress large scan results

---

This structure provides a solid foundation for a professional web security scanner with room for future enhancements and extensions. 