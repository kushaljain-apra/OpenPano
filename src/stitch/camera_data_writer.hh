#pragma once

#include <string>
#include <vector>
#include "camera.hh"

namespace pano {

class CameraDataWriter {
public:
    // Write a vector of cameras with image data to a file
    static void writeCameraData(const std::vector<std::pair<std::string, Mat32f>>& image_data, const std::vector<Camera>& camera_data, const std::string& filename);
};

} // namespace pano