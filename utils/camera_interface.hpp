#ifndef CAMERA_INTERFACE_HPP
#define CAMERA_INTERFACE_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>

class CameraInterface {
private:
    std::vector<cv::VideoCapture> caps;
    std::pair<int, int> resolution;

public:
    CameraInterface(std::pair<int, int> res);
    int cameraCount() const;
    void readFrame(int cameraIndex, cv::Mat &frame);
    void resetAllCameras();
    ~CameraInterface();
};

#endif // CAMERA_INTERFACE_HPP