project(pico-vis)

cmake_minimum_required(VERSION 2.4.0)

# show all warnings
set(CMAKE_CXX_FLAGS "-Wall")

# needed for CV library
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# keep the library path in installed executables
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# build options
option(BUILD_PICOVIS_SENSOR "Build PicoVis sensor server for sending gesture" OFF)
option(BUILD_PICOVIS_VISUALIZER "Build PicoVis cloud visualization client" OFF)

if(BUILD_PICOVIS_SENSOR)
    option(RPI_DEPLOYMENT "Build for Raspberry Pi" OFF)
endif()

# path for additional modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(BUILD_PICOVIS_SENSOR OR BUILD_PICOVIS_VISUALIZER)

    # TUIO Library Directories
    #add_subdirectory(TUIO)
    #add_subdirectory(oscpack)
    include_directories(${pico-vis_SOURCE_DIR}/TUIO)
    include_directories(${pico-vis_SOURCE_DIR}/oscpack)
    #link_directories(${pico-vis_BINARY_DIR}/TUIO)
    #link_directories(${pico-vis_BINARY_DIR}/oscpack)
    link_directories(${pico-vis_BINARY_DIR})
    set(LIBS ${LIBS} TUIO)

    # OpenCV
    FIND_PACKAGE(OpenCV REQUIRED)
    set(LIBS ${LIBS} ${OpenCV_LIBRARIES})

    #OpenNI2
    #Requires OPENNI2 ENV Variable to be set
    include_directories($ENV{OPENNI2_INCLUDE})
    link_directories($ENV{OPENNI2_REDIST})
    set(LIBS ${LIBS} OpenNI2)

    # process config file
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

if(BUILD_PICOVIS_SENSOR)
    set(SRCS ${SRCS}
        src/univ-controller.cpp
        src/HandProcessor.cpp
        src/PicoTuioServer.cpp
        src/Sensor.cpp)

    add_executable(controller ${SRCS})
    target_link_libraries(controller ${LIBS})
endif()

if(BUILD_PICOVIS_VISUALIZER)
    set(SRCS ${SRCS} src/client.cpp)
endif()
