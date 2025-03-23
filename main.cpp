#include <iostream>
#include "utils/gpio_button.hpp"
#include "utils/frame_buffer_display.hpp"
#include "utils/camera_interface.hpp"
#include "utils/camera_manager.hpp"
#include "utils/list_cameras.hpp"
#include <opencv2/opencv.hpp>

// g++ -o build/loop-over main.cpp utils/list_cameras.cpp -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_highgui -I/usr/include/opencv4 $(pkg-config --cflags --libs opencv4)

// WiringPi pin numbers for GPIOs 17, 22, 23, 27
const int BTN_A = 0;  // GPIO17
const int BTN_B = 3;  // GPIO22
const int BTN_C = 4;  // GPIO23
const int BTN_D = 2;  // GPIO27

int main() {
    std::cout << "\e[?25l";
    std::cout << "\e[?12l";  // Disable cursor blinking

    std::vector<int> cameras = getUVCVideoStreams();
    if (cameras.empty()) {
        std::cerr << "No UVC cameras found.\n";
        return 1;
    }

    // buttons: 17, 22, 23, 27
    std::cout << "Buttons: 17, 22, 23, 27" << std::endl;
    // 17: step left, 22: step right, 23: start/stop, 27: exit
    std::cout << "Step left: 17, Step right: 22, Start/Stop: 23, Trigger: 27" << std::endl;

    // // Create 4 buttons
    GpioButton buttonLeft(BTN_A);
    GpioButton buttonRight(BTN_B);
    GpioButton buttonStartStop(BTN_C);
    GpioButton buttonTrigger(BTN_D);

    std::cout << "Cameras: " << cameras.size() << std::endl;

    std::sort(cameras.begin(), cameras.end());

    FramebufferDisplay fbDisplay;  // Initialize framebuffer

    CameraInterface cameraInterface(fbDisplay.getResolution());
    CameraManager cameraManager(cameraInterface.cameraCount());

      // Create bounce pattern: 1 → 2 → 3 → 4 → 3 → 2

    // Open all cameras once

    cameraManager.startLooping();

    while (true) {

        if (buttonLeft.pressed() == 1) {
            cameraManager.cameraLeft();
            std::cout << "Camera left" << std::endl;
        } else if (buttonRight.pressed() == 1) {
            cameraManager.cameraRight();
            std::cout << "Camera right" << std::endl;
        } else if (buttonStartStop.pressed() == 1) {
            std::cout << "Start/Stop" << std::endl;
            if (cameraManager.isLooping()) {
                cameraManager.stopLooping();
            } else {
                cameraManager.startLooping();
            }
        } else if (buttonTrigger.pressed() == 1) {
            std::cout << "Trigger" << std::endl;
            cameraManager.shutter();
        }
        // Long press start stop left right should reorder cameras:
        // left should move current camera left, right should move current camera right
        // in the looping pattern.
        if (buttonStartStop.read()){

        }


        cameraManager.gameLoop();

        int currentCameraIndex = cameraManager.getActiveCamera();
        cv::Mat frame;
        cameraInterface.readFrame(currentCameraIndex, frame);
        fbDisplay.displayFrame(frame, currentCameraIndex, cameraInterface.cameraCount());
    }

    return 0;
}
