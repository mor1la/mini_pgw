cmake_minimum_required(VERSION 3.15)

project(pgw_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Рекурсивный поиск исходников
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# ========== spdlog через FetchContent ==========
include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0 # или нужная стабильная версия
)

FetchContent_MakeAvailable(spdlog)

# Линкуем spdlog
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

# Устанавливаем папку вывода бинарников в папку bin/
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)