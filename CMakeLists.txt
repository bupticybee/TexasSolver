cmake_minimum_required(VERSION 3.20)

project(TexasSolver LANGUAGES CXX CUDA)
# project(TexasSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CUDA_STANDARD 20)
# set(CMAKE_CUDA_STANDARD_REQUIRED ON)
message("${CMAKE_MINOR_VERSION}")
if(${CMAKE_MINOR_VERSION} GREATER_EQUAL 24)
    # set(CMAKE_CUDA_ARCHITECTURES all)
    set(CMAKE_CUDA_ARCHITECTURES all-major)
    # set(CMAKE_CUDA_ARCHITECTURES native)
else()
    set(CMAKE_CUDA_ARCHITECTURES OFF)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=all-major")
endif()
message("CMAKE_CUDA_ARCHITECTURES=${CMAKE_CUDA_ARCHITECTURES}")

message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
if((DEFINED CMAKE_BUILD_TYPE) AND (CMAKE_BUILD_TYPE STREQUAL Debug))
    set(CMAKE_CUDA_FLAGS "-g -G ${CMAKE_CUDA_FLAGS}")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(include)

find_package(OpenMP REQUIRED)
message("OpenMP_CXX_FLAGS=${OpenMP_CXX_FLAGS}")

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler ${OpenMP_CXX_FLAGS}")
message("CMAKE_CUDA_FLAGS=${CMAKE_CUDA_FLAGS}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
set(QT_MAJOR Qt${QT_VERSION_MAJOR})
message("QT_MAJOR=${QT_MAJOR}")
find_package(${QT_MAJOR} REQUIRED COMPONENTS Core Widgets LinguistTools)

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB GUI_SRC *.cpp  src/ui/*.cpp src/runtime/qsolverjob.cpp)
file(GLOB API_SRC src/api.cpp)
file(GLOB EXE_SRC src/console.cpp)
list(REMOVE_ITEM SRC ${GUI_SRC} ${EXE_SRC} ${API_SRC})
# message("SRC=${SRC}")
# message("GUI_SRC=${GUI_SRC}")
# message("API_SRC=${API_SRC}")
# message("EXE_SRC=${EXE_SRC}")
file(GLOB_RECURSE CUDA_SRC src/*.cu)
message("CUDA_SRC=${CUDA_SRC}")

set(BASE_LIB TexasSolver)
add_library(${BASE_LIB} ${SRC} ${CUDA_SRC})
target_link_libraries(${BASE_LIB} PUBLIC ${QT_MAJOR}::Core OpenMP::OpenMP_CXX)
# set_target_properties(${BASE_LIB} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

set(API_TARGET api)
add_library(${API_TARGET} SHARED ${API_SRC})
target_link_libraries(${API_TARGET} PUBLIC ${BASE_LIB})

set(EXE console_solver)
add_executable(${EXE} ${EXE_SRC})
target_link_libraries(${EXE} PRIVATE ${BASE_LIB})
if(MSVC)
    target_link_options(${EXE} PUBLIC "/NODEFAULTLIB:LIBCMT")
endif()

file(GLOB FORMS *.ui)
file(GLOB RESOURCES *.qrc)
file(GLOB TS_FILES *.ts)
file(GLOB QM_FILES *.qm)
# message("FORMS=${FORMS}")
# message("RESOURCES=${RESOURCES}")
# message("TS_FILES=${TS_FILES}")
# message("QM_FILES=${QM_FILES}")

SET(ICON_NAME texassolver_logo)
if(WIN32)
    file(GLOB ICON_FILE imgs/${ICON_NAME}.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}.icns)
    file(GLOB ICON_FILE imgs/${ICON_NAME}.icns)
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()
# message("ICON_FILE=${ICON_FILE}")

# set(CMAKE_AUTOMOC ON) doesn't work
# Q_OBJECT header
file(GLOB HEADERS *.h include/ui/*.h include/runtime/qsolverjob.h)
# message("HEADERS=${HEADERS}")
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt6_wrap_cpp(GUI_SRC ${HEADERS})
else()
    qt5_wrap_cpp(GUI_SRC ${HEADERS})
endif()
# message("GUI_SRC=${GUI_SRC}")

set(GUI TexasSolverGui)
add_executable(${GUI} ${GUI_SRC} ${RESOURCES} ${FORMS} ${ICON_FILE})
target_link_libraries(${GUI} PRIVATE ${QT_MAJOR}::Widgets ${BASE_LIB})
set_target_properties(${GUI} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
