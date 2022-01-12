cmake_minimum_required (VERSION 3.16)
project (googlelog_package_config LANGUAGES CXX)

find_package (googlelog REQUIRED NO_MODULE)

add_executable (googlelog_package_config googlelog_package_config.cc)

target_link_libraries (googlelog_package_config PRIVATE googlelog::googlelog)