cmake_minimum_required(VERSION 3.10)
project(bitget_api)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# Add the executable
add_executable(bitget_api bitget.cpp)

# Link required libraries
target_link_libraries(bitget_api OpenSSL::SSL OpenSSL::Crypto CURL::libcurl)

# Include directories
target_include_directories(bitget_api PRIVATE ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})
