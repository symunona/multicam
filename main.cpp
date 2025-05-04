#include <iostream>
#include "utils/gpio_button.hpp"
#include "utils/frame_buffer_display.hpp"
#include "utils/camera_interface.hpp"
#include "utils/camera_manager.hpp"
#include "utils/list_cameras.hpp"
#include <opencv2/opencv.hpp>

#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include <thread>
#include <cstdlib>


// g++ -o build/loop-over main.cpp utils/list_cameras.cpp -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_highgui -I/usr/include/opencv4 $(pkg-config --cflags --libs opencv4)

// WiringPi pin numbers for GPIOs 17, 22, 23, 27
const int BTN_A = 0;  // GPIO17
const int BTN_B = 3;  // GPIO22
const int BTN_C = 4;  // GPIO23
const int BTN_D = 2;  // GPIO27
const int BTN_SHUTTER = 26;  // GPIO12

// Directories to check
const std::string rawDir = "camera-raw";
const std::string gifDir = "camera-gif";

cv::Mat frames[4];

int initializeAndGetNextIndex() {
    namespace fs = std::filesystem;

    // Create directories if they don't exist
    if (!fs::exists(rawDir)) {
        fs::create_directory(rawDir);
    }
    if (!fs::exists(gifDir)) {
        fs::create_directory(gifDir);
    }

    // Regex to match file names
    std::regex filePattern(R"(MC-(\d{5})-\d+\.jpg|MC-(\d{5})-\d+\.gif)");
    int maxIndex = 0;

    // Lambda to process files in a directory
    auto processDirectory = [&](const std::string& dir) {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::smatch match;
                std::string filename = entry.path().filename().string();
                if (std::regex_search(filename, match, filePattern)) {
                    int index = std::stoi(match[1].str());
                    maxIndex = std::max(maxIndex, index);
                }
            }
        }
    };

    // Process both directories
    processDirectory(rawDir);
    processDirectory(gifDir);

    // Return the next index
    return maxIndex + 1;
}

void saveFramesToRawFolder(cv::Mat frames[], int size, int index) {
    for (int i = 0; i < size; ++i) {
        std::ostringstream filenameStream;
        filenameStream << rawDir << "/MC-" << std::setw(5) << std::setfill('0') << index << "-" << i << ".jpg";
        std::string filename = filenameStream.str();
        if (!frames[i].empty()) {
            cv::imwrite(filename, frames[i]);
            std::cout << "Saved " << filename << std::endl;
        } else {
            std::cerr << "Frame " << i << " is empty, skipping save." << std::endl;
        }
    }
}


void createLoopingGifFromFrames(int index, int numFrames, int delayMs) {
    std::thread([=]() {
        // Construct output filename: gifDir/MC-<index>.gif
        std::ostringstream outputStream;
        outputStream << gifDir << "/MC-" << std::setw(5) << std::setfill('0') << index << ".gif";
        std::string outputPath = outputStream.str();

        // Collect filenames: forward and reverse frames
        std::vector<std::string> framePaths;
        for (int i = 0; i < numFrames; ++i) {
            std::ostringstream pathStream;
            pathStream << rawDir << "/MC-" << std::setw(5) << std::setfill('0') << index << "-" << i << ".jpg";
            framePaths.push_back(pathStream.str());
        }
        for (int i = numFrames - 2; i > 0; --i) {
            std::ostringstream pathStream;
            pathStream << rawDir << "/MC-" << std::setw(5) << std::setfill('0') << index << "-" << i << ".jpg";
            framePaths.push_back(pathStream.str());
        }

        // Build ImageMagick convert command
        std::ostringstream cmdStream;
        cmdStream << "convert -delay " << (delayMs / 10) << " -loop 0";
        for (const auto& path : framePaths) {
            cmdStream << " \"" << path << "\"";
        }
        cmdStream << " \"" << outputPath << "\"";

        std::string command = cmdStream.str();
        std::cout << "[GIF Thread] Executing: " << command << std::endl;

        int result = std::system(command.c_str());
        if (result != 0) {
            std::cerr << "[GIF Thread] ImageMagick convert failed (exit code " << result << ")" << std::endl;
        } else {
            std::cout << "[GIF Thread] GIF created: " << outputPath << std::endl;
        }
    }).detach();  // Fire-and-forget thread
}


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

    // // Create 5 buttons
    GpioButton buttonLeft(BTN_A);
    GpioButton buttonRight(BTN_B);
    GpioButton buttonStartStop(BTN_C);
    GpioButton buttonTrigger(BTN_D);
    GpioButton buttonShutter(BTN_SHUTTER);

    std::cout << "Cameras: " << cameras.size() << std::endl;

    std::sort(cameras.begin(), cameras.end());

    FramebufferDisplay fbDisplay;  // Initialize framebuffer

    // Camera Resolution
    std::pair<int, int> resolution = fbDisplay.getResolution();

    // Set fb resolution to 640x480

    CameraInterface cameraInterface(fbDisplay.getResolution());
    CameraManager cameraManager(cameraInterface.cameraCount());

    int pictureIndex = initializeAndGetNextIndex();

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

        int startTime = cv::getTickCount();
        // Read all camera frames, so they have up-to-date data
        for (int i = 0; i < cameraInterface.cameraCount(); ++i) {
            cameraInterface.readFrame(i, frames[i]);
        }
        int endTime = cv::getTickCount();
        double elapsedTime = (endTime - startTime) / cv::getTickFrequency();
        static int frameCount = 0;
        static double lastTime = cv::getTickCount() / cv::getTickFrequency();
        frameCount++;

        double currentTime = cv::getTickCount() / cv::getTickFrequency();
        if (currentTime - lastTime >= 1.0) {
            double fps = frameCount / (currentTime - lastTime);
            std::cout << "\rFPS: " << fps << "   " << std::flush;
            frameCount = 0;
            lastTime = currentTime;
        }

        // cameraInterface.readFrame(currentCameraIndex, frame);
        fbDisplay.displayFrame(frames[currentCameraIndex], currentIndex, cameraInterface.cameraCount());


        if (buttonShutter.pressed()) {
            std::cout << "Shutter" << std::endl;

            int startTime = cv::getTickCount();
            int index = pictureIndex++;

            saveFramesToRawFolder(frames, cameraInterface.cameraCount(), index);
            createLoopingGifFromFrames(index, cameraInterface.cameraCount(), 200);

            int endTime = cv::getTickCount();
            double elapsedTime = (endTime - startTime) / cv::getTickFrequency();
            std::cout << "Shutter operation completed in " << elapsedTime << " seconds." << std::endl;
            // Apply a shutter effect by fading to black and back
            for (double alpha = 1.0; alpha >= 0.0; alpha -= 0.1) {
                cv::Mat fadedFrame;
                cv::addWeighted(frames[currentCameraIndex], alpha, cv::Mat::zeros(frames[currentCameraIndex].size(), frames[currentCameraIndex].type()), 1.0 - alpha, 0, fadedFrame);
                fbDisplay.displayFrame(fadedFrame, currentIndex, cameraInterface.cameraCount());
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            for (double alpha = 0.0; alpha <= 1.0; alpha += 0.1) {
                cv::Mat fadedFrame;
                cv::addWeighted(frames[currentCameraIndex], alpha, cv::Mat::zeros(frames[currentCameraIndex].size(), frames[currentCameraIndex].type()), 1.0 - alpha, 0, fadedFrame);
                fbDisplay.displayFrame(fadedFrame, currentIndex, cameraInterface.cameraCount());
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }




    }

    return 0;
}
