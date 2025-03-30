# Multi camera controller for the PI

Inspiration is the Nimslo 3D camera.
Take simultaneous pics with n connected cameras.

Order them one after the other, and loop on the preview.

Using PiTFT 2.2 inch non-touch display for now, and the
terminal window, no X. - Setup: https://learn.adafruit.com/adafruit-2-2-pitft-hat-320-240-primary-display-for-raspberry-pi/overview


I use 4 pi zero webcams for now and trying to 


### Compile:

```bash

mkdir build && cd build
cmake ..
make
./camera

```

# Custom Compile

// Do not forget to manually list all the libs at (...)
```bash

g++ -o capture_display main.cpp utils/list_cameras.cpp (...) -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_highgui -I/usr/include/opencv4 $(pkg-config --cflags --libs opencv4)

```

# Libs
Install wiring pi https://github.com/WiringPi/WiringPi?tab=readme-ov-file

wget latest release, then
```bash
sudo apt install ./wiringpi-3.0-1.deb
```

# Cross compilation from a standard linux system

0. Make sure that the raspberry CAN compile the camera.

1. Run `docker build -f Dockerfile.rpi -t rpi-cross .`

2. Run `.cross-compile-setup.sh` on your machine to copy over the files from your raspberry pi to your build machine.

3. Run `docker run --rm -it   -v "$(pwd)":/project   -w /project   rpi-cross`

4. You should get a terminal, then you can navigate to the build folder

`mkdir build && cd build`

5. run `cmake ..` then run `make`

6. finally scp it up with `scp camera rpi:/home/pi/camera`

7. log in to your raspberry, then run `./camera`


