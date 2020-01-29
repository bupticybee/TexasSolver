cmake_minimum_required(VERSION 3.15)
project(TexasSolver)

set(CMAKE_CXX_STANDARD 17)

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
AUX_SOURCE_DIRECTORY(src/tools lib_src)

AUX_SOURCE_DIRECTORY(include lib_src)
AUX_SOURCE_DIRECTORY(include/compairer lib_src)
AUX_SOURCE_DIRECTORY(include/nodes lib_src)
AUX_SOURCE_DIRECTORY(include/tools lib_src)

add_library(TexasSolver
        ${lib_src}
        )

target_link_libraries(TexasSolver fmt)

add_executable(test test/test.cpp)
target_link_libraries(test fmt gtest TexasSolver)
