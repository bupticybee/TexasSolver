cmake_minimum_required(VERSION 3.15)
project(bindSolver)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.7)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra  -fopenmp")
set(CMAKE_CXX_FLAGS "-D_hypot=hypot")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_C_FLAGS "-fopenmp")
set(LINKFLAGS "-fopenmp")

if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif (CMAKE_HOST_WIN64)
    set(WINDOWS 1)
elseif (CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif (CMAKE_HOST_UNIX)
    set(LINUX 1)
endif ()

if (WINDOWS)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()

#openmp
FIND_PACKAGE(OpenMP REQUIRED)
IF(OPENMP_FOUND)
    MESSAGE("openmp found")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF()

# swag
#FIND_PACKAGE(SWIG REQUIRED)
#INCLUDE(${SWIG_USE_FILE})
#FIND_PACKAGE(PythonLibs REQUIRED)
#INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
#SET(CMAKE_SWIG_FLAGS "")

#find_package(PythonLibs 3 REQUIRED)
#MESSAGE("PYTHON PATH: ${PYTHON_INCLUDE_PATH}")
#INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
#find_package(PythonInterp ${PYTHONLIBS_VERSION_STRING} REQUIRED)

#message(STATUS "Python executable is ${PYTHON_EXECUTABLE}")
#message("PYTHONLIBS_VERSION_STRING: ${PYTHONLIBS_VERSION_STRING}")

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

# ppybind11
include_directories(ext/pybind11/include)
add_subdirectory(ext/pybind11)

include_directories(include)

AUX_SOURCE_DIRECTORY(src lib_src)
AUX_SOURCE_DIRECTORY(src/compairer lib_src)
AUX_SOURCE_DIRECTORY(src/nodes lib_src)
AUX_SOURCE_DIRECTORY(src/ranges lib_src)
AUX_SOURCE_DIRECTORY(src/tools lib_src)
AUX_SOURCE_DIRECTORY(src/trainable lib_src)
AUX_SOURCE_DIRECTORY(src/solver lib_src)
AUX_SOURCE_DIRECTORY(src/runtime lib_src)

AUX_SOURCE_DIRECTORY(include lib_src)
AUX_SOURCE_DIRECTORY(include/compairer lib_src)
AUX_SOURCE_DIRECTORY(include/nodes lib_src)
AUX_SOURCE_DIRECTORY(include/ranges lib_src)
AUX_SOURCE_DIRECTORY(include/tools lib_src)
AUX_SOURCE_DIRECTORY(include/trainable lib_src)
AUX_SOURCE_DIRECTORY(include/solver lib_src)
AUX_SOURCE_DIRECTORY(include/runtime lib_src)

add_library(TexasSolver
        ${lib_src}
        )

target_link_libraries(TexasSolver fmt)
#target_link_libraries(TexasSolver ${Boost_LIBRARIES})

add_executable(test test/test.cpp)
add_executable(console_solver src/console.cpp)
target_link_libraries(test gtest TexasSolver)
target_link_libraries(console_solver TexasSolver)

#set_property(SOURCE swig/PokerSolver.i PROPERTY CPLUSPLUS ON)
#SET_SOURCE_FILES_PROPERTIES(PokerSolver.i PROPERTIES CPLUSPLUS ON)
#SET_SOURCE_FILES_PROPERTIES(PokerSolver.i PROPERTIES SWIG_FLAGS "")
#SWIG_ADD_LIBRARY(PokerSolver TYPE SHARED LANGUAGE python SOURCES PokerSolver.i)
#set_target_properties(PokerSolver PROPERTIES LIBRARY_OUTPUT_DIRECTORY ./output)
##SWIG_ADD_MODULE(PokerSolver python PokerSolver.i ${lib_src})
#SWIG_LINK_LIBRARIES(PokerSolver TexasSolver ${PYTHON_LIBRARIES})

INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/resources" DESTINATION "${PROJECT_SOURCE_DIR}/install" OPTIONAL)
INSTALL(TARGETS console_solver DESTINATION "${PROJECT_SOURCE_DIR}/install")
