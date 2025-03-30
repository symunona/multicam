#!/bin/bash

# PI_HOST="q2"   # Replace with your Pi's IP or hostname

# this is currently BROKEN! Could not figure out, but the
# docker starter throws an /usr/bin/bash: /usr/bin/bash: cannot execute binary file
# error when called with the bash -c flag.
# Possibly I am doing something wrong with mounting the file system, overwriting
# the bash binary in the docker container with the one from the host system.
# Did not figure how to fix it YET. Until then, just run the container, and use
# the bash shell inside the container to run the build commands.

# this file should also ssh and upload the binary file.

docker run --rm -it \
  -v "$(pwd)":/project \
  -w /project/build \
  rpi-cross \
  /usr/bin/bash -c "
    mkdir -p build && cd build"
    && \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../pi-toolchain.cmake \
             -DOpenCV_DIR=/project/.rpi-sysroot/usr/lib/aarch64-linux-gnu/cmake/opencv4 && \
    make -j\$(nproc)
  "