cmake_minimum_required(VERSION 3.22)
project(EvE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(EvE ${SRC_FILES})

target_include_directories(EvE PRIVATE "include")

target_link_libraries(EvE PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)