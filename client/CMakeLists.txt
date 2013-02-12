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
include_directories("/home/pi/test/OpenNI2/Include")
#link_directories ("/home/pi/test/OpenNI2/Bin/Armv6l-Release")
link_directories(".")
set(LIBS ${LIBS} OpenNI2)

# TUIO
include_directories("/home/pi/test/TUIO_CPP/TUIO")
include_directories("/home/pi/test/TUIO_CPP/oscpack")
link_directories ("/home/pi/test/TUIO_CPP")

FIND_PACKAGE(OpenCV REQUIRED)

set(SRCS
    client.cpp
    Sensor.cpp
    HandProcessor.cpp
)

add_executable(pico-vis ${SRCS})

target_link_libraries(pico-vis ${LIBS} ${OpenCV_LIBS} TUIO)
