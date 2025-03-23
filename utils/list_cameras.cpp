#include <vector>
#include <string>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <libudev.h>
#include <cstring>
#include <map>
#include <algorithm>
#include <iostream>

std::vector<int> getUVCVideoStreams() {
    std::map<int, std::string> devNumToSerial;

    // --- Get serials using libudev ---
    struct udev *udev = udev_new();
    if (!udev) {
        std::cerr << "Failed to initialize udev\n";
        return {};
    }

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "video4linux");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    std::map<std::string, std::string> devPathToSerial;

    udev_list_entry_foreach(entry, devices) {
        const char *syspath = udev_list_entry_get_name(entry);
        struct udev_device *dev = udev_device_new_from_syspath(udev, syspath);

        const char *devNode = udev_device_get_devnode(dev);
        const char *serial = udev_device_get_property_value(dev, "ID_SERIAL_SHORT");
        if (devNode && serial) {
            devPathToSerial[devNode] = serial;
        }

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    // --- Filter UVC video streams and match with serials ---
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
                        auto it = devPathToSerial.find(devPath);
                        if (it != devPathToSerial.end()) {
                            devNumToSerial[devNum] = it->second;
                        }
                    }
                }
            }
        }
        close(fd);
    }

    // --- Sort by serial and return device numbers ---
    std::vector<std::pair<int, std::string>> sorted;
    for (const auto& [devNum, serial] : devNumToSerial) {
        sorted.emplace_back(devNum, serial);
    }

    std::sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b) {
        return a.second < b.second; // sort by serial
    });

    std::vector<int> orderedDevices;
    for (const auto &[devNum, _] : sorted) {
        orderedDevices.push_back(devNum);
    }

    return orderedDevices;
}
