cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++0x)

try_run(gs_RUN gs_COMPILE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/gen-signals.cpp)

add_subdirectory(lisle/sys)
