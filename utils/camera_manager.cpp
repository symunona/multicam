#include "camera_manager.hpp"
#include <iostream>
#include <stdexcept>

CameraManager::CameraManager(int cameraCount)
    : activeCameraIndex(0), looping(false), totalCameras(cameraCount),
      direction(Direction::Right), lastSwitchTime(std::chrono::steady_clock::now()) {
    if (cameraCount <= 1) {
        throw std::invalid_argument("Camera count must be greater than 1.");
    }
}

void CameraManager::startLooping() {
    looping = true;
    lastSwitchTime = std::chrono::steady_clock::now();
    std::cout << "Looping started\n";
}

void CameraManager::stopLooping() {
    looping = false;
    std::cout << "Looping stopped\n";
}

bool CameraManager::isLooping() const {
    return looping;
}

void CameraManager::cameraLeft() {
    activeCameraIndex = (activeCameraIndex - 1 + totalCameras) % totalCameras;
    direction = Direction::Left;
    looping = false;
    lastSwitchTime = std::chrono::steady_clock::now();
}

void CameraManager::cameraRight() {
    activeCameraIndex = (activeCameraIndex + 1) % totalCameras;
    direction = Direction::Right;
    looping = false;
    lastSwitchTime = std::chrono::steady_clock::now();
}

int CameraManager::getActiveCamera() const {
    return activeCameraIndex;
}

void CameraManager::shutter() {
    std::cout << "Shutter triggered (stub) for camera " << activeCameraIndex << "\n";
}

void CameraManager::gameLoop() {
    if (!looping) return;

    auto now = std::chrono::steady_clock::now();
    if (now - lastSwitchTime < stepInterval) return;

    // Handle ping-pong pattern: 0 → 1 → 2 → 3 → 2 → 1 → ...
    if (direction == Direction::Right) {
        if (activeCameraIndex < totalCameras - 1) {
            activeCameraIndex++;
        } else {
            direction = Direction::Left;
            activeCameraIndex--;
        }
    } else { // Direction::Left
        if (activeCameraIndex > 0) {
            activeCameraIndex--;
        } else {
            direction = Direction::Right;
            activeCameraIndex++;
        }
    }

    lastSwitchTime = now;
}