# WebEye PowerShell Build Script
# This script builds the WebEye security scanner on Windows

param(
    [string]$Action = "all"
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

# Function to install vcpkg if not present
function Install-Vcpkg {
    Write-Status "Checking for vcpkg..."
    
    if (Test-Path "C:\vcpkg\vcpkg.exe") {
        Write-Success "vcpkg found at C:\vcpkg"
        return "C:\vcpkg\vcpkg.exe"
    }
    
    Write-Status "vcpkg not found. Installing..."
    
    # Clone vcpkg
    if (-not (Test-Path "C:\vcpkg")) {
        git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
    }
    
    # Bootstrap vcpkg
    & "C:\vcpkg\bootstrap-vcpkg.bat"
    
    if (Test-Path "C:\vcpkg\vcpkg.exe") {
        Write-Success "vcpkg installed successfully"
        return "C:\vcpkg\vcpkg.exe"
    } else {
        Write-Error "Failed to install vcpkg"
        exit 1
    }
}

# Function to install dependencies
function Install-Dependencies {
    Write-Status "Installing dependencies..."
    
    $vcpkg = Install-Vcpkg
    
    # Install curl
    Write-Status "Installing libcurl..."
    & $vcpkg install curl:x64-windows
    
    # Install nlohmann-json
    Write-Status "Installing nlohmann-json..."
    & $vcpkg install nlohmann-json:x64-windows
    
    Write-Success "Dependencies installed successfully"
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
    
    if (-not (Test-Command "git")) {
        $missing += "git"
    }
    
    if ($missing.Count -gt 0) {
        Write-Error "Missing dependencies: $($missing -join ', ')"
        Write-Status "Please install the missing dependencies:"
        Write-Host "  - CMake: https://cmake.org/download/" -ForegroundColor $Yellow
        Write-Host "  - MinGW: https://www.mingw-w64.org/downloads/" -ForegroundColor $Yellow
        Write-Host "  - Git: https://git-scm.com/download/win" -ForegroundColor $Yellow
        exit 1
    }
    
    Write-Success "All basic dependencies found"
}

# Function to build the project
function Build-Project {
    Write-Status "Building WebEye..."
    
    # Create build directory
    if (-not (Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }
    
    Set-Location "build"
    
    # Configure with CMake
    Write-Status "Configuring with CMake..."
    $vcpkg = Install-Vcpkg
    $vcpkgRoot = "C:\vcpkg"
    
    cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="$vcpkgRoot\scripts\buildsystems\vcpkg.cmake"
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake configuration failed"
        Set-Location ".."
        exit 1
    }
    
    # Build the project
    Write-Status "Compiling..."
    cmake --build .
    
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

# Function to install the binary
function Install-Binary {
    Write-Status "Installing WebEye..."
    
    $executable = "build\bin\webeye.exe"
    if (-not (Test-Path $executable)) {
        $executable = "build\webeye.exe"
    }
    
    if (-not (Test-Path $executable)) {
        Write-Error "Executable not found. Cannot install."
        exit 1
    }
    
    # Create installation directory
    $installDir = "C:\Program Files\WebEye"
    if (-not (Test-Path $installDir)) {
        New-Item -ItemType Directory -Path $installDir -Force | Out-Null
    }
    
    # Copy binary
    Copy-Item $executable "$installDir\webeye.exe" -Force
    
    # Add to PATH
    $currentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
    if ($currentPath -notlike "*$installDir*") {
        $newPath = "$currentPath;$installDir"
        [Environment]::SetEnvironmentVariable("PATH", $newPath, "Machine")
    }
    
    Write-Success "WebEye installed to $installDir\webeye.exe"
    Write-Warning "You may need to restart your terminal for PATH changes to take effect"
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
    Write-Host "WebEye PowerShell Build Script" -ForegroundColor $Green
    Write-Host ""
    Write-Host "Usage:"
    Write-Host "  .\build.ps1 [OPTIONS]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Action <action>    Action to perform (default: all)"
    Write-Host ""
    Write-Host "Available Actions:"
    Write-Host "  check-deps          Check if dependencies are installed"
    Write-Host "  install-deps        Install system dependencies"
    Write-Host "  build               Build the project"
    Write-Host "  test                Run basic tests"
    Write-Host "  install             Install binary to system"
    Write-Host "  all                 Check deps, build, test, and install (default)"
    Write-Host "  clean               Clean build files"
    Write-Host "  help                Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build.ps1                    # Full build and install"
    Write-Host "  .\build.ps1 -Action install-deps # Install dependencies only"
    Write-Host "  .\build.ps1 -Action build      # Build only"
    Write-Host "  .\build.ps1 -Action test       # Run tests only"
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
Write-Host "    WebEye PowerShell Build Script v1.0" -ForegroundColor $Blue
Write-Host ""

# Parse action
switch ($Action.ToLower()) {
    "check-deps" {
        Test-Dependencies
    }
    "install-deps" {
        Test-Dependencies
        Install-Dependencies
    }
    "build" {
        Test-Dependencies
        Build-Project
    }
    "test" {
        Test-Build
    }
    "install" {
        Install-Binary
    }
    "all" {
        Test-Dependencies
        Install-Dependencies
        Build-Project
        Test-Build
        Install-Binary
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
Write-Success "WebEye build process completed!"
Write-Host ""
Write-Host "Usage examples:" -ForegroundColor $Yellow
Write-Host "  webeye --url https://example.com" -ForegroundColor $White
Write-Host "  webeye --url https://example.com --export results.json --verbose" -ForegroundColor $White
Write-Host "  webeye --url https://example.com --timeout 15" -ForegroundColor $White
Write-Host "  webeye --help" -ForegroundColor $White
Write-Host "" 