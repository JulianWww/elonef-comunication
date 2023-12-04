cmake_minimum_required(VERSION 3.25.0)
project(elonef-communication LANGUAGES C CXX)

set(USE_TLS TRUE)

find_package(Git REQUIRED)

if (NOT EXISTS "cryptopp-cmake")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/abdes/cryptopp-cmake.git
        WORKING_DIRECTORY "./"
    )
endif()

if (NOT EXISTS "cryptopp-pem-cmake")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/JulianWww/cryptopp-pem-cmake
        WORKING_DIRECTORY "./"
    )
endif()

if (NOT EXISTS "uuid_v4")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/crashoz/uuid_v4
        WORKING_DIRECTORY "./"
    )
endif()

if (NOT EXISTS "BinarySearchTree")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/JulianWww/BinarySearchTree
        WORKING_DIRECTORY "./"
    )
endif()

if (NOT EXISTS "json")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/nlohmann/json
        WORKING_DIRECTORY "./"
    )
endif()

if (NOT EXISTS "IXWebSocket")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone https://github.com/JulianWww/IXWebSocket
        WORKING_DIRECTORY "./"
    )
else()
execute_process(
    COMMAND "${GIT_EXECUTABLE}" pull https://github.com/JulianWww/IXWebSocket
    WORKING_DIRECTORY "IXWebSocket"
)
endif()

find_package(OpenSSL REQUIRED)


set(CMAKE_SKIP_INSTALL_RULES TRUE)
add_subdirectory(cryptopp-cmake)
add_subdirectory(cryptopp-pem-cmake)
add_subdirectory(uuid_v4)
add_subdirectory(BinarySearchTree)
add_subdirectory(json)
add_subdirectory(IXWebSocket)


FILE(GLOB_RECURSE source src/*)
file(GLOB_RECURSE TEST "test/*.cpp")

include(CTest)
enable_testing()


add_library(elonef-communication ${source})
target_link_libraries(elonef-communication PUBLIC cryptopp cryptopp-pem uuid_v4 binary-search-tree nlohmann_json::nlohmann_json ixwebsocket)

set_property(TARGET elonef-communication PROPERTY CXX_STANDARD 23)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


target_include_directories(elonef-communication PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(test_suit ${TEST}) 
target_link_libraries(test_suit elonef-communication cryptopp cryptopp-pem ixwebsocket)
set_property(TARGET test_suit PROPERTY CXX_STANDARD 23)
add_compile_definitions(ASIO_STANDALONE=1)