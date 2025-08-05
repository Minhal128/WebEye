#!/bin/bash

# WebEye Build Script
# This script automates the build and installation process

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install dependencies
install_dependencies() {
    print_status "Checking and installing dependencies..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Linux
        if command_exists apt-get; then
            # Ubuntu/Debian
            sudo apt-get update
            sudo apt-get install -y build-essential cmake pkg-config git
            if ! command_exists curl; then
                sudo apt-get install -y libcurl4-openssl-dev
            fi
        elif command_exists yum; then
            # CentOS/RHEL
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y cmake pkg-config git
            if ! command_exists curl; then
                sudo yum install -y libcurl-devel
            fi
        elif command_exists pacman; then
            # Arch Linux
            sudo pacman -S --needed base-devel cmake pkg-config git
            if ! command_exists curl; then
                sudo pacman -S curl
            fi
        else
            print_warning "Could not detect package manager. Please install dependencies manually."
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if command_exists brew; then
            brew install cmake pkg-config
            if ! command_exists curl; then
                brew install curl
            fi
        else
            print_warning "Homebrew not found. Please install dependencies manually."
        fi
    else
        print_warning "Unsupported operating system. Please install dependencies manually."
    fi
}

# Function to build the project
build_project() {
    print_status "Building WebEye..."
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure with CMake
    print_status "Configuring with CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Build
    print_status "Compiling..."
    if command_exists nproc; then
        make -j$(nproc)
    else
        make -j4
    fi
    
    cd ..
}

# Function to run tests
run_tests() {
    print_status "Running basic tests..."
    
    if [ -f "build/bin/webeye" ]; then
        # Test help command
        if ./build/bin/webeye --help >/dev/null 2>&1; then
            print_success "Help command works"
        else
            print_error "Help command failed"
            return 1
        fi
        
        # Test with a safe target
        print_status "Testing with httpbin.org..."
        if timeout 30s ./build/bin/webeye https://httpbin.org/get >/dev/null 2>&1; then
            print_success "Basic scan test passed"
        else
            print_warning "Basic scan test failed (this might be normal if network is restricted)"
        fi
    else
        print_error "WebEye binary not found"
        return 1
    fi
}

# Function to install
install_binary() {
    print_status "Installing WebEye..."
    
    if [ -f "build/bin/webeye" ]; then
        sudo cp build/bin/webeye /usr/local/bin/
        sudo chmod +x /usr/local/bin/webeye
        print_success "WebEye installed to /usr/local/bin/webeye"
    else
        print_error "WebEye binary not found"
        return 1
    fi
}

# Function to clean build files
clean_build() {
    print_status "Cleaning build files..."
    rm -rf build
    print_success "Build files cleaned"
}

# Function to show help
show_help() {
    echo "WebEye Build Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --install-deps    Install system dependencies"
    echo "  --build          Build the project"
    echo "  --test           Run basic tests"
    echo "  --install        Install binary to system"
    echo "  --all            Install deps, build, test, and install (default)"
    echo "  --clean          Clean build files"
    echo "  --help           Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 --all         # Full build and install"
    echo "  $0 --build       # Build only"
    echo "  $0 --clean       # Clean build files"
}

# Main script logic
main() {
    print_status "WebEye Build Script Starting..."
    
    # Parse command line arguments
    if [ $# -eq 0 ]; then
        # No arguments, run full build
        install_dependencies
        build_project
        run_tests
        install_binary
        print_success "WebEye build and installation completed successfully!"
        echo ""
        echo "You can now use WebEye:"
        echo "  webeye --help"
        echo "  webeye https://example.com"
        return 0
    fi
    
    case "$1" in
        --install-deps)
            install_dependencies
            ;;
        --build)
            build_project
            ;;
        --test)
            run_tests
            ;;
        --install)
            install_binary
            ;;
        --all)
            install_dependencies
            build_project
            run_tests
            install_binary
            print_success "WebEye build and installation completed successfully!"
            ;;
        --clean)
            clean_build
            ;;
        --help|-h)
            show_help
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@" 