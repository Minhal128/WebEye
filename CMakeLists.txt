cmake_minimum_required(VERSION 3.16)
project(WebEye VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags for optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add nlohmann/json as a single header (much faster)
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

# Try to find CURL, but don't fail if not found
find_package(CURL QUIET)

# If CURL not found, try to find it manually
if(NOT CURL_FOUND)
    # Try to find CURL using pkg-config or manual paths
    find_path(CURL_INCLUDE_DIR curl/curl.h
        PATHS
        "C:/Program Files/curl/include"
        "C:/curl/include"
        "C:/Program Files (x86)/curl/include"
        "C:/Windows/System32"
        "C:/Windows/SysWOW64"
        "C:/msys64/mingw64/include"
        "C:/msys64/usr/include"
    )
    
    find_library(CURL_LIBRARY
        NAMES libcurl curl
        PATHS
        "C:/Program Files/curl/lib"
        "C:/curl/lib"
        "C:/Program Files (x86)/curl/lib"
        "C:/Windows/System32"
        "C:/Windows/SysWOW64"
        "C:/msys64/mingw64/lib"
        "C:/msys64/usr/lib"
    )
    
    if(CURL_INCLUDE_DIR AND CURL_LIBRARY)
        set(CURL_FOUND TRUE)
        set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR})
        set(CURL_LIBRARIES ${CURL_LIBRARY})
        message(STATUS "CURL found manually: TRUE")
        set(USE_SIMPLE_HTTP OFF)
    else()
        message(WARNING "CURL not found. Please install libcurl development libraries.")
        message(STATUS "For Windows: Download from https://curl.se/windows/")
        message(STATUS "For Linux: sudo apt-get install libcurl4-openssl-dev")
        message(STATUS "For macOS: brew install curl")
        message(STATUS "Using simple HTTP client implementation")
        set(USE_SIMPLE_HTTP ON)
    endif()
else()
    message(STATUS "CURL found: ${CURL_VERSION}")
    set(USE_SIMPLE_HTTP OFF)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/scanner.cpp
    src/utils.cpp
)

# Add simple HTTP files if using simple HTTP mode
if(USE_SIMPLE_HTTP)
    list(APPEND SOURCES src/simple_http.cpp)
    add_definitions(-DUSE_SIMPLE_HTTP)
endif()

# Create executable
add_executable(webeye ${SOURCES})

# Link libraries based on what's available
if(CURL_FOUND)
    if(CURL_LIBRARIES)
        # Manually found CURL
        target_include_directories(webeye PRIVATE ${CURL_INCLUDE_DIRS})
        target_link_libraries(webeye 
            ${CURL_LIBRARIES}
            nlohmann_json::nlohmann_json
        )
    else()
        # Found via find_package
        target_link_libraries(webeye 
            CURL::libcurl
            nlohmann_json::nlohmann_json
        )
    endif()
else()
    target_link_libraries(webeye 
        nlohmann_json::nlohmann_json
        ws2_32  # Windows sockets
        iphlpapi # Windows IP helper
        wsock32 # Additional Windows socket library
    )
endif()

# Set output directory
set_target_properties(webeye PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install target
install(TARGETS webeye
    RUNTIME DESTINATION bin
)

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "CURL found: ${CURL_FOUND}")
message(STATUS "Use simple HTTP: ${USE_SIMPLE_HTTP}")

# Add custom target for cleaning
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files"
) 