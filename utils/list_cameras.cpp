#include <iostream>
#include <vector>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <cstring>


std::vector<int> getUVCVideoStreams() {
    std::vector<int> videoStreams;
    for (const auto &entry : std::filesystem::directory_iterator("/dev/")) {
        std::string devPath = entry.path().string();
        if (devPath.find("/dev/video") == std::string::npos) continue;

        int fd = open(devPath.c_str(), O_RDWR);
        if (fd < 0) continue;

        struct v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == 0) {
            if (strstr((char*)cap.driver, "uvcvideo")) {
                if ((cap.device_caps & V4L2_CAP_VIDEO_CAPTURE) && 
                    !(cap.device_caps & V4L2_CAP_META_CAPTURE)) {

                    struct v4l2_fmtdesc fmt;
                    memset(&fmt, 0, sizeof(fmt));
                    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    
                    bool validFormat = false;
                    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0) {
                        if (fmt.pixelformat == V4L2_PIX_FMT_MJPEG ||
                            fmt.pixelformat == V4L2_PIX_FMT_YUYV ||
                            fmt.pixelformat == V4L2_PIX_FMT_H264) {
                            validFormat = true;
                            break;
                        }
                        fmt.index++;
                    }

                    if (validFormat) {
                        int devNum = std::stoi(devPath.substr(10));
                        videoStreams.push_back(devNum);
                    }
                }
            }
        }
        close(fd);
    }
    return videoStreams;
}