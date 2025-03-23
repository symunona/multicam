#include "camera_manager.hpp"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

CameraManager::CameraManager(int cameraCount)
    : activeCameraIndex(0), looping(false), totalCameras(cameraCount),
      direction(Direction::Right), lastSwitchTime(std::chrono::steady_clock::now()) {
    if (cameraCount <= 1) {
        throw std::invalid_argument("Camera count must be greater than 1.");
    }
    loadOrCreateCameraOrder();
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

int CameraManager::getMappedCameraIndex() const {
    return mapCameraOrder(activeCameraIndex);
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



void CameraManager::loadOrCreateCameraOrder() {
    const std::string filename = "camera_order";

    cameraDeviceOrder.clear();

    if (std::filesystem::exists(filename)) {
        std::ifstream infile(filename);
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            int devNum;
            if (iss >> devNum) {
                cameraDeviceOrder.push_back(devNum);
            }
        }

        if (cameraDeviceOrder.size() != static_cast<size_t>(totalCameras)) {
            std::cerr << "Invalid camera_order file. Resetting to default.\n";
            cameraDeviceOrder.clear();
        }
    }

    if (cameraDeviceOrder.empty()) {
        for (int i = 0; i < totalCameras; ++i) {
            cameraDeviceOrder.push_back(i);
        }
        saveCameraOrder();
    }

    std::cout << "Camera order: ";
    for (int cam : cameraDeviceOrder) std::cout << cam << " ";
    std::cout << "\n";
}

void CameraManager::saveCameraOrder() const {
    std::ofstream outfile("camera_order");
    for (int cam : cameraDeviceOrder) {
        outfile << cam << "\n";
    }
}

const std::vector<int>& CameraManager::getCameraDeviceOrder() const {
    return cameraDeviceOrder;
}

int CameraManager::mapCameraOrder(int logicalIndex) const {
    if (logicalIndex < 0 || logicalIndex >= static_cast<int>(cameraDeviceOrder.size())) {
        throw std::out_of_range("Invalid logical camera index.");
    }
    return cameraDeviceOrder[logicalIndex];
}

void CameraManager::swap(int i, int j) {
    if (i < 0 || j < 0 || i >= totalCameras || j >= totalCameras) return;
    std::swap(cameraDeviceOrder[i], cameraDeviceOrder[j]);
    if (activeCameraIndex == i) activeCameraIndex = j;
    else if (activeCameraIndex == j) activeCameraIndex = i;
    saveCameraOrder();
}

void CameraManager::moveLeft() {
    if (activeCameraIndex > 0) {
        std::swap(cameraDeviceOrder[activeCameraIndex], cameraDeviceOrder[activeCameraIndex - 1]);
        activeCameraIndex--;
        saveCameraOrder();
    }
}

void CameraManager::moveRight() {
    if (activeCameraIndex < totalCameras - 1) {
        std::swap(cameraDeviceOrder[activeCameraIndex], cameraDeviceOrder[activeCameraIndex + 1]);
        activeCameraIndex++;
        saveCameraOrder();
    }
}
