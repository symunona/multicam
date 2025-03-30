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
    // std::cout << "Buttons: 17, 22, 23, 27" << std::endl;
    // 17: step left, 22: step right, 23: start/stop, 27: exit
    // std::cout << "Step left: 17, Step right: 22, Start/Stop: 23, Trigger: 27" << std::endl;

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

        bool rightPressed = buttonRight.pressed();
        bool leftPressed = buttonLeft.pressed();
        bool buttonStartStopPressed = buttonStartStop.isLongPressed();

        // Long press start stop left right should reorder cameras:
        // left should move current camera left, right should move current camera right
        // in the looping pattern.
        if (buttonStartStop.read()){
            std::cout << "Start/Stop held" << std::endl;
            if (rightPressed) {
                std::cout << "Camera MOVED right" << std::endl;
                cameraManager.moveRight();
            } else if (leftPressed) {
                cameraManager.moveLeft();
                std::cout << "Camera MOVED left" << std::endl;
            }
            if (! leftPressed && ! rightPressed && buttonStartStopPressed) {
                std::cout << "Start/Stop" << std::endl;
                if (cameraManager.isLooping()) {
                    cameraManager.stopLooping();
                } else {
                    cameraManager.startLooping();
                }
            }
        } else {
            if (leftPressed) {
                cameraManager.cameraLeft();
                std::cout << "Camera left" << std::endl;
            } else if (rightPressed) {
                cameraManager.cameraRight();
                std::cout << "Camera right" << std::endl;
            } else if (buttonTrigger.pressed()) {
                std::cout << "Trigger" << std::endl;
                cameraManager.shutter();
                return 0;
            }
        }


        cameraManager.gameLoop();

        int currentIndex = cameraManager.getActiveCamera();
        int currentCameraIndex = cameraManager.getMappedCameraIndex();
        cv::Mat frames[4];

        int startTime = cv::getTickCount();
        // Read all camera frames, so they have up-to-date data
        for (int i = 0; i < cameraInterface.cameraCount(); ++i) {
            cameraInterface.readFrame(i, frames[i]);
        }
        int endTime = cv::getTickCount();
        double elapsedTime = (endTime - startTime) / cv::getTickFrequency();
        std::cout << "\rFPS: " << 1.0 / elapsedTime << "   " << std::flush;

        // cameraInterface.readFrame(currentCameraIndex, frame);
        fbDisplay.displayFrame(frames[currentCameraIndex], currentIndex, cameraInterface.cameraCount());
    }

    return 0;
}
