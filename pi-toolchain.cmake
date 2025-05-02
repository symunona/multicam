# set(CMAKE_SYSTEM_NAME Linux)
# set(CMAKE_SYSTEM_PROCESSOR armv7l)  # Change to aarch64 for 64-bit
# set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
# set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)  # 64-bit ARM

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

set(SYSROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/.rpi-sysroot")

# Proper sysroot support
set(CMAKE_SYSROOT ${SYSROOT_PATH})
set(CMAKE_FIND_ROOT_PATH ${SYSROOT_PATH})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# OpenCV path (optional if find_package is used)
set(OpenCV_DIR "${SYSROOT_PATH}/usr/lib/aarch64-linux-gnu/cmake/opencv4")

# Help pkg-config find things
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${SYSROOT_PATH})
set(ENV{PKG_CONFIG_LIBDIR} ${SYSROOT_PATH}/usr/lib/aarch64-linux-gnu/pkgconfig)

# Optional: tell CMake where to look
include_directories(${SYSROOT_PATH}/usr/include)
link_directories(${SYSROOT_PATH}/usr/lib/aarch64-linux-gnu)
