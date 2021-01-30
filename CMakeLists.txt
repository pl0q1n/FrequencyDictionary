cmake_minimum_required(VERSION 3.5)
project(freq)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(abseil-cpp)

add_executable(freq main.cpp app.h mmap_file.h fast_string.h)
target_link_libraries(freq absl::base absl::flat_hash_map)

target_compile_options(freq PRIVATE "-O3")
