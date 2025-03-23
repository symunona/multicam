#include "camera_interface.hpp"
#include "list_cameras.hpp"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <algorithm>

CameraInterface::CameraInterface(std::pair<int, int> res) : resolution(res) {
    // Initialize cameras
    std::vector<int> cameras = getUVCVideoStreams();
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
        throw std::runtime_error("Failed to capture frame from camera " + std::to_string(cameraIndex));
    }
}