cmake_minimum_required(VERSION 3.15)
project(TexasSolver)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra  -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# google test
include_directories(ext/googletest/googletest/include ext/googletest/googletest include)
add_subdirectory(ext/googletest/googletest)

# string format
include_directories(ext/fmt/include ext/fmt)
add_subdirectory(ext/fmt/)

include_directories(include)

AUX_SOURCE_DIRECTORY(src lib_src)
AUX_SOURCE_DIRECTORY(src/compairer lib_src)
AUX_SOURCE_DIRECTORY(src/nodes lib_src)
AUX_SOURCE_DIRECTORY(src/ranges lib_src)
AUX_SOURCE_DIRECTORY(src/tools lib_src)
AUX_SOURCE_DIRECTORY(src/trainable lib_src)
AUX_SOURCE_DIRECTORY(src/solver lib_src)

AUX_SOURCE_DIRECTORY(include lib_src)
AUX_SOURCE_DIRECTORY(include/compairer lib_src)
AUX_SOURCE_DIRECTORY(include/nodes lib_src)
AUX_SOURCE_DIRECTORY(include/ranges lib_src)
AUX_SOURCE_DIRECTORY(include/tools lib_src)
AUX_SOURCE_DIRECTORY(include/trainable lib_src)
AUX_SOURCE_DIRECTORY(include/solver lib_src)

add_library(TexasSolver
        ${lib_src}
        )

target_link_libraries(TexasSolver fmt)

add_executable(test test/test.cpp)
target_link_libraries(test fmt gtest TexasSolver)
