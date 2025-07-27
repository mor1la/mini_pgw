cmake_minimum_required(VERSION 3.15)
project(pgw_server)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.0+5
)
FetchContent_MakeAvailable(spdlog crow)

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

add_executable(pgw_server
    main.cpp
    PgwServer.cpp
    CdrWriter.cpp
    HttpServer.cpp
    SessionManager.cpp
    UdpServer.cpp
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

target_include_directories(pgw_server PRIVATE
    ../common
    ${crow_SOURCE_DIR}/include
)

target_link_libraries(pgw_server PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json)
set_target_properties(pgw_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
