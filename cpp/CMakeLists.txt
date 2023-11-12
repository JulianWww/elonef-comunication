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

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/BinarySearchTree")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/JulianWww/BinarySearchTree
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/json")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/nlohmann/json
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()



set(CMAKE_SKIP_INSTALL_RULES TRUE)
add_subdirectory(cryptopp-cmake)
add_subdirectory(cryptopp-pem-cmake)
add_subdirectory(uuid_v4)
add_subdirectory(BinarySearchTree)
add_subdirectory(json)

FILE(GLOB_RECURSE source src/*)
file(GLOB_RECURSE TEST "test/*.cpp")

include(CTest)
enable_testing()


add_library(elonef-communication ${source})
target_link_libraries(elonef-communication cryptopp cryptopp-pem uuid_v4 binary-search-tree nlohmann_json::nlohmann_json)
target_include_directories(elonef-communication PUBLIC websocket/)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


target_include_directories(elonef-communication PUBLIC ${CMAKE_SOURCE_DIR}/include)
set_property(TARGET elonef-communication PROPERTY CXX_STANDARD 17)

add_executable(test_suit ${TEST}) 
target_link_libraries(test_suit elonef-communication cryptopp cryptopp-pem)
set_property(TARGET test_suit PROPERTY CXX_STANDARD 23)

add_executable(server echo_server.cpp)
add_executable(client echo_client.cpp)
target_include_directories(server PUBLIC websocket/)
target_include_directories(client PUBLIC websocket/)

add_compile_definitions(ASIO_STANDALONE=1)