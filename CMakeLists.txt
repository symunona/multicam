cmake_minimum_required(VERSION 3.10)
project(MultiCam3D)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Source files
set(SOURCES
    main.cpp
    utils/list_cameras.cpp
    utils/gpio_button.cpp
    utils/camera_interface.cpp
    utils/camera_manager.cpp
)

# Add executable
add_executable(camera ${SOURCES})

# Link OpenCV libraries
target_link_libraries(camera ${OpenCV_LIBS} wiringPi)