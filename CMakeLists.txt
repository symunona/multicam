cmake_minimum_required(VERSION 3.10)
project(MultiCam3D)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Find libudev using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUDEV REQUIRED libudev)

include_directories(${LIBUDEV_INCLUDE_DIRS})
link_directories(${LIBUDEV_LIBRARY_DIRS})

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

# Link libraries
target_link_libraries(camera
    ${OpenCV_LIBS}
    wiringPi
    ${LIBUDEV_LIBRARIES}
)