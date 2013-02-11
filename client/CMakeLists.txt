project(pico-vis)

cmake_minimum_required(VERSION 2.4.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cpp")

# needed for CV library
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# enable debugging, and show all warnings
set(CMAKE_CXX_FLAGS "-g -Wall")

# OpenNI
include_directories("/Users/brandt/Desktop/work/test/OpenNI2/Redist/OpenNI-2.1.0-x64/Include")
link_directories ("/Users/brandt/Desktop/work/test/OpenNI2/Redist/OpenNI-2.1.0-x64/Redist")
set(LIBS ${LIBS} OpenNI2)

# TUIO
include_directories("/Users/brandt/Downloads/TUIO_CPP/TUIO")
include_directories("/Users/brandt/Downloads/TUIO_CPP/oscpack")
link_directories ("/Users/brandt/Downloads/TUIO_CPP")
set(LIBS ${LIBS} OpenNI2)

FIND_PACKAGE(OpenCV REQUIRED)

set(SRCS
    client.cpp
    Sensor.cpp
    HandProcessor.cpp
)

add_executable(pico-vis ${SRCS})

target_link_libraries(pico-vis ${LIBS} ${OpenCV_LIBS} TUIO)
