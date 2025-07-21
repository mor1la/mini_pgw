cmake_minimum_required(VERSION 3.5)

project(maze)

file(GLOB_RECURSE SOURCES "*.cpp")

file(GLOB_RECURSE HEADERS "*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED TRUE
)