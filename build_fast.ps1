# WebEye Fast Build Script
# This script builds WebEye quickly without external dependencies

param(
    [string]$Action = "build"
)

# Colors for output
$Red = "Red"
$Green = "Green"
$Yellow = "Yellow"
$Blue = "Blue"
$White = "White"

# Function to print colored output
function Write-Status {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor $Blue
}

function Write-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor $Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARNING] $Message" -ForegroundColor $Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor $Red
}

# Function to check if command exists
function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    }
    catch {
        return $false
    }
}

# Function to check dependencies
function Test-Dependencies {
    Write-Status "Checking dependencies..."
    
    $missing = @()
    
    if (-not (Test-Command "cmake")) {
        $missing += "cmake"
    }
    
    if (-not (Test-Command "g++")) {
        $missing += "g++"
    }
    
    if ($missing.Count -gt 0) {
        Write-Error "Missing dependencies: $($missing -join ', ')"
        Write-Status "Please install the missing dependencies:"
        Write-Host "  - CMake: https://cmake.org/download/" -ForegroundColor $Yellow
        Write-Host "  - MinGW: https://www.mingw-w64.org/downloads/" -ForegroundColor $Yellow
        exit 1
    }
    
    Write-Success "All dependencies found"
}

# Function to build the project
function Build-Project {
    Write-Status "Building WebEye (Fast Mode)..."
    
    # Create build directory
    if (-not (Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }
    
    Set-Location "build"
    
    # Configure with CMake (using simple HTTP mode)
    Write-Status "Configuring with CMake (Simple HTTP Mode)..."
    cmake .. -G "MinGW Makefiles" -DUSE_SIMPLE_HTTP=ON
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake configuration failed"
        Set-Location ".."
        exit 1
    }
    
    # Build the project
    Write-Status "Compiling..."
    cmake --build . --config Release
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed"
        Set-Location ".."
        exit 1
    }
    
    Set-Location ".."
    Write-Success "Build completed successfully!"
}

# Function to test the build
function Test-Build {
    Write-Status "Running basic tests..."
    
    $executable = "build\bin\webeye.exe"
    if (-not (Test-Path $executable)) {
        $executable = "build\webeye.exe"
    }
    
    if (-not (Test-Path $executable)) {
        Write-Error "Executable not found. Build may have failed."
        exit 1
    }
    
    # Test help command
    & $executable --help | Out-Null
    if ($LASTEXITCODE -eq 0) {
        Write-Success "Help command works"
    } else {
        Write-Error "Help command failed"
        exit 1
    }
    
    Write-Success "Basic tests passed"
}

# Function to run a quick scan
function Test-Scan {
    Write-Status "Testing WebEye with a quick scan..."
    
    $executable = "build\bin\webeye.exe"
    if (-not (Test-Path $executable)) {
        $executable = "build\webeye.exe"
    }
    
    if (-not (Test-Path $executable)) {
        Write-Error "Executable not found. Please build first."
        exit 1
    }
    
    Write-Status "Running test scan on httpbin.org..."
    & $executable --url "http://httpbin.org/get" --timeout 5
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "Test scan completed successfully!"
    } else {
        Write-Warning "Test scan had issues (this is normal for some targets)"
    }
}

# Function to clean build files
function Clean-Build {
    Write-Status "Cleaning build files..."
    if (Test-Path "build") {
        Remove-Item "build" -Recurse -Force
    }
    Write-Success "Build files cleaned"
}

# Function to show usage
function Show-Usage {
    Write-Host "WebEye Fast Build Script" -ForegroundColor $Green
    Write-Host ""
    Write-Host "Usage:"
    Write-Host "  .\build_fast.ps1 [OPTIONS]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Action <action>    Action to perform (default: build)"
    Write-Host ""
    Write-Host "Available Actions:"
    Write-Host "  check-deps          Check if dependencies are installed"
    Write-Host "  build               Build the project (default)"
    Write-Host "  test                Run basic tests"
    Write-Host "  scan                Run a test scan"
    Write-Host "  clean               Clean build files"
    Write-Host "  help                Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build_fast.ps1              # Build WebEye"
    Write-Host "  .\build_fast.ps1 -Action test # Run tests"
    Write-Host "  .\build_fast.ps1 -Action scan # Test scan"
}

# Main script logic
Write-Host ""
Write-Host " __      __      ___.    _________ .__                 " -ForegroundColor $Blue
Write-Host "/  \    /  \ ____\_ |__  /   _____/|  |   ____  ____  " -ForegroundColor $Blue
Write-Host "\   \/\/   // __ \| __ \ \_____  \ |  | _/ __ \/  _ \ " -ForegroundColor $Blue
Write-Host " \        /\  ___/| \_\ \/        \|  |_\  ___(  <_> )" -ForegroundColor $Blue
Write-Host "  \__/\  /  \___  >___  /_______  /|____/\___  >____/ " -ForegroundColor $Blue
Write-Host "       \/       \/    \/        \/           \/        " -ForegroundColor $Blue
Write-Host ""
Write-Host "    WebEye Fast Build Script v1.0" -ForegroundColor $Blue
Write-Host ""

# Parse action
switch ($Action.ToLower()) {
    "check-deps" {
        Test-Dependencies
    }
    "build" {
        Test-Dependencies
        Build-Project
    }
    "test" {
        Test-Build
    }
    "scan" {
        Test-Scan
    }
    "clean" {
        Clean-Build
    }
    "help" {
        Show-Usage
    }
    default {
        Write-Error "Unknown action: $Action"
        Show-Usage
        exit 1
    }
}

Write-Host ""
Write-Success "WebEye fast build process completed!"
Write-Host ""
Write-Host "Usage examples:" -ForegroundColor $Yellow
Write-Host "  .\build\bin\webeye.exe --url https://example.com" -ForegroundColor $White
Write-Host "  .\build\bin\webeye.exe --url https://example.com --export results.json --verbose" -ForegroundColor $White
Write-Host "  .\build\bin\webeye.exe --url https://example.com --timeout 15" -ForegroundColor $White
Write-Host "  .\build\bin\webeye.exe --help" -ForegroundColor $White
Write-Host "" 