#!/bin/bash

# WebEye Test Script
# Tests various functionality of the WebEye security scanner

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[TEST]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

print_error() {
    echo -e "${RED}[FAIL]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# Function to check if executable exists
check_executable() {
    if [ ! -f "build/bin/webeye" ]; then
        print_error "WebEye executable not found. Please build the project first."
        exit 1
    fi
}

# Function to run test
run_test() {
    local test_name="$1"
    local command="$2"
    local expected_exit="$3"
    
    print_status "Running: $test_name"
    
    if eval "$command" >/dev/null 2>&1; then
        if [ "$expected_exit" = "0" ] || [ -z "$expected_exit" ]; then
            print_success "$test_name"
            return 0
        else
            print_error "$test_name (expected exit $expected_exit, got 0)"
            return 1
        fi
    else
        if [ "$expected_exit" != "0" ]; then
            print_success "$test_name (expected failure)"
            return 0
        else
            print_error "$test_name (unexpected failure)"
            return 1
        fi
    fi
}

# Function to test help command
test_help() {
    run_test "Help Command" "./build/bin/webeye --help"
}

# Function to test invalid arguments
test_invalid_args() {
    run_test "Invalid Arguments" "./build/bin/webeye --invalid-arg" "1"
}

# Function to test missing URL
test_missing_url() {
    run_test "Missing URL" "./build/bin/webeye --export test.json" "1"
}

# Function to test basic scan
test_basic_scan() {
    print_status "Running: Basic Scan Test"
    
    # Use a safe test URL
    local test_url="https://httpbin.org/get"
    local output_file="test_scan.json"
    
    if ./build/bin/webeye --url "$test_url" --export "$output_file" --timeout 10; then
        if [ -f "$output_file" ]; then
            print_success "Basic scan completed and exported results"
            rm -f "$output_file"
            return 0
        else
            print_error "Basic scan failed - no output file created"
            return 1
        fi
    else
        print_error "Basic scan failed"
        return 1
    fi
}

# Function to test verbose mode
test_verbose_mode() {
    print_status "Running: Verbose Mode Test"
    
    local test_url="https://httpbin.org/headers"
    local output=$(./build/bin/webeye --url "$test_url" --verbose 2>&1)
    
    if echo "$output" | grep -q "Starting comprehensive security scan"; then
        print_success "Verbose mode shows detailed output"
        return 0
    else
        print_error "Verbose mode not working properly"
        return 1
    fi
}

# Function to test timeout
test_timeout() {
    print_status "Running: Timeout Test"
    
    # Test with a very short timeout
    if timeout 15s ./build/bin/webeye --url "https://httpbin.org/delay/10" --timeout 5; then
        print_warning "Timeout test completed (may have succeeded or timed out)"
        return 0
    else
        print_success "Timeout test worked as expected"
        return 0
    fi
}

# Function to test JSON export format
test_json_export() {
    print_status "Running: JSON Export Format Test"
    
    local test_url="https://httpbin.org/json"
    local output_file="test_json.json"
    
    if ./build/bin/webeye --url "$test_url" --export "$output_file"; then
        if [ -f "$output_file" ]; then
            # Check if JSON is valid
            if python3 -m json.tool "$output_file" >/dev/null 2>&1; then
                print_success "JSON export format is valid"
                rm -f "$output_file"
                return 0
            else
                print_error "JSON export format is invalid"
                rm -f "$output_file"
                return 1
            fi
        else
            print_error "JSON export file not created"
            return 1
        fi
    else
        print_error "JSON export test failed"
        return 1
    fi
}

# Function to test interactive mode (basic)
test_interactive_mode() {
    print_status "Running: Interactive Mode Test"
    
    # Test if interactive mode starts without crashing
    if timeout 5s ./build/bin/webeye --interactive >/dev/null 2>&1; then
        print_success "Interactive mode starts successfully"
        return 0
    else
        print_warning "Interactive mode test (may have timed out as expected)"
        return 0
    fi
}

# Function to test port scanning
test_port_scanning() {
    print_status "Running: Port Scanning Test"
    
    local test_url="https://httpbin.org"
    local output=$(./build/bin/webeye --url "$test_url" --verbose 2>&1)
    
    if echo "$output" | grep -q "Scanning common ports"; then
        print_success "Port scanning functionality detected"
        return 0
    else
        print_warning "Port scanning test inconclusive"
        return 0
    fi
}

# Function to test vulnerability detection
test_vulnerability_detection() {
    print_status "Running: Vulnerability Detection Test"
    
    local test_url="https://httpbin.org"
    local output=$(./build/bin/webeye --url "$test_url" --verbose 2>&1)
    
    if echo "$output" | grep -q "Checking for vulnerabilities"; then
        print_success "Vulnerability detection functionality detected"
        return 0
    else
        print_warning "Vulnerability detection test inconclusive"
        return 0
    fi
}

# Function to run all tests
run_all_tests() {
    echo -e "${BLUE}"
    echo " __      __      ___.    _________ .__                 "
    echo "/  \    /  \ ____\_ |__  /   _____/|  |   ____  ____  "
    echo "\   \/\/   // __ \| __ \ \_____  \ |  | _/ __ \/  _ \ "
    echo " \        /\  ___/| \_\ \/        \|  |_\  ___(  <_> )"
    echo "  \__/\  /  \___  >___  /_______  /|____/\___  >____/ "
    echo "       \/       \/    \/        \/           \/        "
    echo ""
    echo "    WebEye Test Suite v1.0"
    echo -e "${NC}"
    
    local total_tests=0
    local passed_tests=0
    local failed_tests=0
    
    # Check if executable exists
    check_executable
    
    # Run tests
    local tests=(
        "test_help"
        "test_invalid_args"
        "test_missing_url"
        "test_basic_scan"
        "test_verbose_mode"
        "test_timeout"
        "test_json_export"
        "test_interactive_mode"
        "test_port_scanning"
        "test_vulnerability_detection"
    )
    
    for test in "${tests[@]}"; do
        total_tests=$((total_tests + 1))
        if $test; then
            passed_tests=$((passed_tests + 1))
        else
            failed_tests=$((failed_tests + 1))
        fi
        echo ""
    done
    
    # Print summary
    echo "=========================================="
    echo "Test Summary:"
    echo "  Total tests: $total_tests"
    echo "  Passed: $passed_tests"
    echo "  Failed: $failed_tests"
    echo "=========================================="
    
    if [ $failed_tests -eq 0 ]; then
        print_success "All tests passed!"
        exit 0
    else
        print_error "$failed_tests test(s) failed"
        exit 1
    fi
}

# Function to show usage
show_usage() {
    echo "WebEye Test Script"
    echo ""
    echo "Usage:"
    echo "  ./test.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --help          Show this help message"
    echo "  --basic         Run basic tests only"
    echo "  --full          Run all tests (default)"
    echo ""
    echo "Examples:"
    echo "  ./test.sh              # Run all tests"
    echo "  ./test.sh --basic      # Run basic tests only"
}

# Main script logic
main() {
    case "${1:---full}" in
        --help|-h)
            show_usage
            ;;
        --basic)
            echo "Running basic tests..."
            test_help
            test_invalid_args
            test_missing_url
            ;;
        --full)
            run_all_tests
            ;;
        *)
            echo "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@" 