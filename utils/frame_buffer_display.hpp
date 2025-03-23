#ifndef FRAMEBUFFER_DISPLAY_HPP
#define FRAMEBUFFER_DISPLAY_HPP

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <opencv2/opencv.hpp>

class FramebufferDisplay {
private:
    int fbfd;                 // Framebuffer file descriptor
    void *fbp;                // Mapped framebuffer memory
    int screensize;           // Size of the framebuffer
    int width, height;        // Target framebuffer resolution


public:
    std::pair<int, int> getResolution(){
        return std::make_pair(width, height);
    }
    // Constructor initializes the framebuffer
    FramebufferDisplay() : fbfd(-1), fbp(nullptr), screensize(0), width(0), height(0) {
        fbfd = open("/dev/fb1", O_RDWR);
        if (fbfd < 0) {
            std::cerr << "[FrameBufferDisplay] Error: Cannot open framebuffer\n";
            return;
        }



        // Get framebuffer screen info
        struct fb_var_screeninfo vinfo;
        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
            std::cerr << "[FrameBufferDisplay] Error: Cannot get framebuffer info\n";
            close(fbfd);
            return;
        }

        width = vinfo.xres;
        height = vinfo.yres;
        screensize = width * height * (vinfo.bits_per_pixel / 8);

        std::cout << "[FrameBufferDisplay] resolution: " << width << "x" << height << "\n";

        // Map framebuffer memory
        fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
        if (fbp == MAP_FAILED) {
            std::cerr << "[FrameBufferDisplay] Error: Framebuffer mapping failed\n";
            close(fbfd);
            fbp = nullptr;
            return;
        }
    }

    // Destructor cleans up resources
    ~FramebufferDisplay() {
        if (fbp && fbp != MAP_FAILED) {
            munmap(fbp, screensize);
        }
        if (fbfd >= 0) {
            close(fbfd);
        }
    }

    // Display a frame (assumes correct size & format)
    bool displayFrame(const cv::Mat &frame) {
        if (!fbp || fbfd < 0) {
            std::cerr << "[FrameBufferDisplay] Error: Framebuffer not initialized\n";
            return false;
        }

        // Resize to framebuffer size
        cv::Mat resized;
        cv::resize(frame, resized, cv::Size(width, height));

        // Convert to BGR565 (for direct framebuffer writing)
        cv::Mat bgr565;
        cv::cvtColor(resized, bgr565, cv::COLOR_BGR2BGR565);

        // Copy data to framebuffer
        memcpy(fbp, bgr565.data, screensize);
        return true;
    }


    bool displayFrame(cv::Mat &frame, int camIndex, int totalCams) {
        if (!fbp || fbfd < 0) {
            std::cerr << "[FrameBufferDisplay] Error: Framebuffer not initialized\n";
            return false;
        }

        // Resize frame to match framebuffer size
        cv::Mat resized;
        cv::resize(frame, resized, cv::Size(width, height));

        // Draw an indicator bar on top
        int barY = 10;  // Position from top
        int barHeight = 10;
        int segmentWidth = width / totalCams;

        for (int i = 0; i < totalCams; ++i) {
            cv::Scalar color = (i == camIndex) ? cv::Scalar(255, 255, 255) : cv::Scalar(0, 0, 0);  // White for active camera
            cv::rectangle(resized, cv::Point(i * segmentWidth, barY), cv::Point((i + 1) * segmentWidth, barY + barHeight), color, cv::FILLED);
        }

        // Convert to BGR565 for framebuffer
        cv::Mat bgr565;
        cv::cvtColor(resized, bgr565, cv::COLOR_BGR2BGR565);

        // Copy to framebuffer
        memcpy(fbp, bgr565.data, screensize);

        return true;
    }

    bool showHud() {
        if (!fbp || fbfd < 0) {
            std::cerr << "[FrameBufferDisplay] Error: Framebuffer not initialized\n";
            return false;
        }

        // Draw a simple HUD
        cv::Mat hud = cv::Mat::zeros(height, width, CV_16UC1);
        cv::putText(hud, "Hello, World!", cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);

        // Copy to framebuffer
        memcpy(fbp, hud.data, screensize);

        return true;
    }
};

#endif // FRAMEBUFFER_DISPLAY_HPP
