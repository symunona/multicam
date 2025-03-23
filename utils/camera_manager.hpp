#ifndef CAMERA_MANAGER_HPP
#define CAMERA_MANAGER_HPP

#include <chrono>

class CameraManager {
public:
    enum class Direction { Left, Right };

    CameraManager(int cameraCount);

    void startLooping();
    void stopLooping();
    bool isLooping() const;

    void cameraLeft();
    void cameraRight();
    int getActiveCamera() const;
    void shutter();
    void gameLoop();

private:
    int activeCameraIndex;
    bool looping;
    int totalCameras;
    Direction direction;
    std::chrono::steady_clock::time_point lastSwitchTime;
    const std::chrono::milliseconds stepInterval = std::chrono::milliseconds(400); // Example interval
};

#endif // CAMERA_MANAGER_HPP