cmake_minimum_required(VERSION 3.25.0)
project(elonef-communication LANGUAGES C CXX)


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

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/websocket")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/JulianWww/websocket
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()


set(CMAKE_SKIP_INSTALL_RULES TRUE)
add_subdirectory(cryptopp-cmake)
add_subdirectory(cryptopp-pem-cmake)
add_subdirectory(uuid_v4)


FILE(GLOB_RECURSE source src/*)
file(GLOB_RECURSE TEST "test/*.cpp")

include(CTest)
enable_testing()


add_library(elonef-communication elonef-communication.cpp ${source})
target_link_libraries(elonef-communication cryptopp cryptopp-pem uuid_v4)
target_include_directories(elonef-communication PUBLIC websocket/ )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


target_include_directories(elonef-communication PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(test_suit ${TEST}) 
target_link_libraries(test_suit elonef-communication cryptopp cryptopp-pem)

add_executable(server echo_server.cpp)
add_executable(client echo_client.cpp)
target_include_directories(server PUBLIC websocket/)
target_include_directories(client PUBLIC websocket/)

add_compile_definitions(ASIO_STANDALONE=1)