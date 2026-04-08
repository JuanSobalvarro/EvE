cmake_minimum_required(VERSION 3.22)
project(EvE)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL3 REQUIRED)

add_executable(EvE src/main.cpp src/game.cpp src/entity.cpp src/ernest.cpp src/world.cpp)

target_include_directories(EvE PRIVATE "include")

target_link_libraries(EvE PRIVATE SDL3::SDL3)