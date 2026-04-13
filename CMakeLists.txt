cmake_minimum_required(VERSION 3.22)
project(EvE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent) # fetch dependencies

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(Lua REQUIRED)
find_package(OpenAL REQUIRED)   

FetchContent_Declare(
    sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.3.0
)

FetchContent_Declare(
    dr_libs
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
    GIT_TAG master
)
FetchContent_MakeAvailable(sol2 dr_libs)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(EvE ${SRC_FILES})

target_include_directories(EvE PRIVATE 
    "include"
    ${LUA_INCLUDE_DIR}
    ${dr_libs_SOURCE_DIR}
)

target_link_libraries(EvE PRIVATE 
    SDL3::SDL3 
    SDL3_image::SDL3_image 
    ${LUA_LIBRARIES}
    sol2::sol2
    OpenAL::OpenAL
)