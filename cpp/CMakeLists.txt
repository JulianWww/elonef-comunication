cmake_minimum_required(VERSION 3.27.6)
project(elonef-communication VERSION 0.1.0 LANGUAGES C CXX)

file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

find_package(Git REQUIRED)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/cryptopp-cmake")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/abdes/cryptopp-cmake.git
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/cryptopp-pem-cmake")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/JulianWww/cryptopp-pem-cmake
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/uuid_v4")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/crashoz/uuid_v4
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

set(CMAKE_SKIP_INSTALL_RULES TRUE)
add_subdirectory(cryptopp-cmake)

set(CMAKE_SKIP_INSTALL_RULES TRUE)
add_subdirectory(cryptopp-pem-cmake)

set(CMAKE_SKIP_INSTALL_RULES TRUE)
add_subdirectory(uuid_v4)


CPMAddPackage(
  NAME websocketpp
  GITHUB_REPOSITORY zaphoyd/websocketpp
  GIT_TAG 0.8.2
) 

FILE(GLOB_RECURSE source src/*)
file(GLOB_RECURSE TEST "test/*.cpp")

include(CTest)
enable_testing()

add_library(elonef-communication elonef-communication.cpp ${source})
target_link_libraries(elonef-communication cryptopp cryptopp-pem uuid_v4)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


target_include_directories(elonef-communication PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(test_suit ${TEST}) 
target_link_libraries(test_suit elonef-communication cryptopp cryptopp-pem)