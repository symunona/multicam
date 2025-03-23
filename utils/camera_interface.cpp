#include "camera_interface.hpp"
#include "list_cameras.hpp"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <algorithm>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

CameraInterface::CameraInterface(std::pair<int, int> res) : resolution(res) {
    // Initialize cameras
    std::vector<int> cameras = getUVCVideoStreams();

    // resetAllCameras();

    if (cameras.empty()) {
        throw std::runtime_error("No UVC cameras found");
    }
    std::sort(cameras.begin(), cameras.end());

    // Setup all cameras
    for (int cam : cameras) {
        caps.emplace_back("/dev/video" + std::to_string(cam));
        if (!caps.back().isOpened()) {
            throw std::runtime_error("Failed to open /dev/video" + std::to_string(cam));
        }

        caps.back().set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        caps.back().set(cv::CAP_PROP_FRAME_WIDTH, resolution.first);
        caps.back().set(cv::CAP_PROP_FRAME_HEIGHT, resolution.second);
        caps.back().set(cv::CAP_PROP_FPS, 4);
        caps.back().set(cv::CAP_PROP_BUFFERSIZE, 3);
    }
}

int CameraInterface::cameraCount() const {
    return caps.size();
}

void CameraInterface::readFrame(int cameraIndex, cv::Mat &frame) {
    if (cameraIndex < 0 || cameraIndex >= caps.size()) {
        throw std::runtime_error("Camera index out of range");
    }
    caps[cameraIndex] >> frame;
    if (frame.empty()) {
        std::cerr << ("Failed to capture frame from camera " + std::to_string(cameraIndex)) << "\n";
        frame = cv::Mat::zeros(resolution.second, resolution.first, CV_8UC3);
    }
}

CameraInterface::~CameraInterface() {
    for (auto& cap : caps) {
        if (cap.isOpened()) {
            cap.release();
        }
    }
}


void CameraInterface::resetAllCameras() {
    for (const auto& entry : std::filesystem::directory_iterator("/dev/")) {
        std::string devPath = entry.path().string();
        if (devPath.find("/dev/video") == std::string::npos) continue;

        int fd = open(devPath.c_str(), O_RDWR);
        if (fd < 0) {
            std::cerr << "Could not open " << devPath << ": " << strerror(errno) << "\n";
            continue;
        }

        struct v4l2_capability cap{};
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == 0 &&
            strstr((char*)cap.driver, "uvcvideo") != nullptr) {

            std::cout << "Resetting " << devPath << "\n";

            // Soft reset example: reset basic controls to defaults
            struct v4l2_control ctrl;
            std::vector<int> controlsToReset = {
                V4L2_CID_BRIGHTNESS,
                V4L2_CID_CONTRAST,
                V4L2_CID_SATURATION,
                V4L2_CID_HUE,
                V4L2_CID_GAIN,
                V4L2_CID_EXPOSURE_AUTO,
                V4L2_CID_EXPOSURE_ABSOLUTE
            };

            for (int id : controlsToReset) {
                memset(&ctrl, 0, sizeof(ctrl));
                ctrl.id = id;
                ctrl.value = 0; // This value might not always make sense — could query default
                if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) != 0) {
                    // Not all controls are supported — ignore errors
                }
            }
        }

        close(fd);
    }
}