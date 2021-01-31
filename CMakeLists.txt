cmake_minimum_required(VERSION 3.5)
project(FreqDictionary)

set(CMAKE_CXX_STANDARD 17)

add_executable(FreqDictionary main.cpp app.h mmap_file.h utils.h 3rdParty/pqsort.h)

if (FAST)
add_definitions(-DGOTTA_GO_FAST)
add_subdirectory(3rdParty/abseil-cpp)
target_link_libraries(FreqDictionary absl::flat_hash_map)
endif()