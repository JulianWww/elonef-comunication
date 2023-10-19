cmake_minimum_required(VERSION 3.27.6)
project(elonef-communication VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_library(elonef-communication elonef-communication.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_executable(test_suit test/main_test.cpp)
