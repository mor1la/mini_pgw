cmake_minimum_required(VERSION 3.15)

project(pgw_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.0+5
)
FetchContent_MakeAvailable(crow)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE COMMON_SOURCES
    src/common/*.cpp
)

file(GLOB_RECURSE COMMON_HEADERS
    src/common/*.h
    src/common/*.hpp
)

file(GLOB_RECURSE SERVER_SOURCES
    src/server/*.cpp
)

add_executable(pgw_server
    ${SERVER_SOURCES}
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

target_link_libraries(pgw_server PRIVATE
    spdlog::spdlog
)

target_include_directories(pgw_server PRIVATE
    ${crow_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    src/common
)

set_target_properties(pgw_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

file(GLOB_RECURSE CLIENT_SOURCES
    src/client/*.cpp
)

add_executable(pgw_client
    ${CLIENT_SOURCES}
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

target_link_libraries(pgw_client PRIVATE
    spdlog::spdlog
)

target_include_directories(pgw_client PRIVATE
    ${crow_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    src/common
)

set_target_properties(pgw_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

enable_testing()

add_executable(
    unit_tests
    src/tests/SessionManagerTest.cpp
    src/tests/mocks/MockCdrWriter.h
    src/server/SessionManager.cpp 
    src/server/CdrWriter.cpp 
    src/common
    src/server
    src/client
)

target_link_libraries(unit_tests
    gtest gmock gtest_main
    pthread
)

target_include_directories(unit_tests PRIVATE
    ${crow_SOURCE_DIR}/include
    ${spdlog_SOURCE_DIR}/include       
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
)


add_test(NAME AllTests COMMAND unit_tests)