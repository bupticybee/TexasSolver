cmake_minimum_required(VERSION 3.15)
project(TexasSolver)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.7)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra  -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_C_FLAGS "-fopenmp")
set(LINKFLAGS "-fopenmp")

#openmp
FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
    MESSAGE("openmp found")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF()

#set (Boost_NO_SYSTEM_PATHS ON)
#set (Boost_USE_MULTITHREADED ON)
#set (Boost_USE_STATIC_LIBS ON)
#set (Boost_USE_STATIC_RUNTIME OFF)
#set (BOOST_ALL_DYN_LINK OFF)

#ADD_DEFINITIONS (-DBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION)
#ADD_DEFINITIONS (-DBOOST_THREAD_PROVIDES_EXECUTORS)
#ADD_DEFINITIONS (-DBOOST_THREAD_USES_MOVE)
#ADD_DEFINITIONS (-DBOOST_THREAD_PROVIDES_FUTURE)


#find_package(Boost 1.72.0 COMPONENTS system filesystem thread REQUIRED)
#set(Boost_USE_STATIC_LIBS ON)
#include_directories(${Boost_INCLUDE_DIRS})

#message(STATUS "BOOST INCLUDE DIRS: ${Boost_INCLUDE_DIRS}")
#message(STATUS "BOOST LIBRARYS: ${Boost_LIBRARIES}")

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

add_library(TexasSolver SHARED
        ${lib_src}
        )

target_link_libraries(TexasSolver fmt)
#target_link_libraries(TexasSolver ${Boost_LIBRARIES})

add_executable(test test/test.cpp)
target_link_libraries(test gtest TexasSolver)
