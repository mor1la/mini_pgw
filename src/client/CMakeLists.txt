cmake_minimum_required(VERSION 3.15)
project(pgw_client)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Общие исходники
file(GLOB_RECURSE COMMON_SOURCES
    ../common/*.cpp
)
file(GLOB_RECURSE COMMON_HEADERS
    ../common/*.h
    ../common/*.hpp
)

add_executable(pgw_client
    main.cpp
    UdpClient.cpp
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

target_include_directories(pgw_client PRIVATE
    ../common
)

target_link_libraries(pgw_client PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json)
set_target_properties(pgw_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
