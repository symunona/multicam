#ifndef CAMERA_MANAGER_HPP
#define CAMERA_MANAGER_HPP

#include <chrono>
#include <vector>

#define CAMERA_SWITCH_INTERVAL 200 // milliseconds

enum class Direction {
    Left,   
    Right
};

class CameraManager {
    public:
        CameraManager(int cameraCount);

        void startLooping();
        void stopLooping();
        bool isLooping() const;

        void cameraLeft();
        void cameraRight();
        int getActiveCamera() const;
        int getMappedCameraIndex() const;
        void shutter();
        void gameLoop();

        // New
        void swap(int i, int j);
        void moveLeft();
        void moveRight();
        int mapCameraOrder(int logicalIndex) const;
        const std::vector<int>& getCameraDeviceOrder() const;

    private:
        int activeCameraIndex;
        bool looping;
        int totalCameras;
        Direction direction;
        std::chrono::steady_clock::time_point lastSwitchTime;
        const std::chrono::milliseconds stepInterval{CAMERA_SWITCH_INTERVAL};

        std::vector<int> cameraDeviceOrder;

        void loadOrCreateCameraOrder();
        void saveCameraOrder() const;
};

#endif // CAMERA_MANAGER_HPP