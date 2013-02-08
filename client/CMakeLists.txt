project(pico-vis)

cmake_minimum_required(VERSION 2.4.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cpp")

# enable debugging, and show all warnings
set(CMAKE_CXX_FLAGS "-g -Wall")

# OpenNI
include_directories("/Users/brandt/Desktop/work/test/OpenNI2/Redist/OpenNI-2.1.0-x64/Include")
link_directories ("/Users/brandt/Desktop/work/test/OpenNI2/Redist/OpenNI-2.1.0-x64/Redist")
set(LIBS ${LIBS} OpenNI2)

#FIND_PACKAGE(OpenCV REQUIRED)

set(SRCS
    client.cpp
    sensor.cpp
)

add_executable(pico-vis ${SRCS})

target_link_libraries(pico-vis ${LIBS})