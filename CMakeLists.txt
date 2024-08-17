cmake_minimum_required(VERSION 3.10)

project(CLIProgressBarTest)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(SOURCE_FILE CLIProgressBarTest.cpp)

add_executable(CLIProgressBarTest ${SOURCE_FILE})