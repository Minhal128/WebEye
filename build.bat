@echo off
setlocal enabledelayedexpansion

REM WebEye Build Script for Windows
REM This script automates the build process on Windows

echo [INFO] WebEye Build Script Starting...

REM Check if CMake is available
cmake --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake not found. Please install CMake from https://cmake.org/download/
    echo [INFO] You can also install it via: winget install Kitware.CMake
    pause
    exit /b 1
)

REM Check if Visual Studio or Build Tools are available
where cl >nul 2>&1
if errorlevel 1 (
    echo [WARNING] Visual Studio compiler not found in PATH
    echo [INFO] Please run this script from a Visual Studio Developer Command Prompt
    echo [INFO] Or install Visual Studio Build Tools
    echo [INFO] You can also try: "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    pause
)

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo [INFO] Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo [ERROR] CMake configuration failed
    cd ..
    pause
    exit /b 1
)

REM Build the project
echo [INFO] Building WebEye...
cmake --build . --config Release
if errorlevel 1 (
    echo [ERROR] Build failed
    cd ..
    pause
    exit /b 1
)

cd ..

REM Check if binary was created
if exist "build\bin\Release\webeye.exe" (
    echo [SUCCESS] WebEye built successfully!
    echo.
    echo [INFO] Binary location: build\bin\Release\webeye.exe
    echo.
    echo [INFO] Testing help command...
    build\bin\Release\webeye.exe --help >nul 2>&1
    if errorlevel 1 (
        echo [WARNING] Help command test failed
    ) else (
        echo [SUCCESS] Help command works
    )
    echo.
    echo [INFO] You can now use WebEye:
    echo   build\bin\Release\webeye.exe --help
    echo   build\bin\Release\webeye.exe https://example.com
) else (
    echo [ERROR] WebEye binary not found
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build completed successfully!
pause 