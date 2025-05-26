#include <fstream>
#include <iomanip>
#include <vector>
#include "camera_data_writer.hh"

namespace pano {

void CameraDataWriter::writeCameraData(const std::vector<std::pair<std::string, Mat32f>>& image_data, const std::vector<Camera>& camera_data, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Write each camera's data
    for (int i = 0; i < (int)camera_data.size(); i++) {
        // Write image path
        outFile << image_data[i].first << "\n";
        
        // Write image dimensions
        outFile << image_data[i].second.width() << " " << image_data[i].second.height() << "\n\n";
        
        // Write camera matrix (K)
        outFile << "1 0 " << std::fixed << std::setprecision(1) << image_data[i].second.height() / (float)2 << "\n";
        outFile << "0 1 " << std::fixed << std::setprecision(1) << image_data[i].second.width() / (float)2 << "\n";
        outFile << "0 0 1" << "\n";
        outFile << "\n";
        
        // Write rotation matrix
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                outFile << std::fixed << std::setprecision(10) << camera_data[i].R[row * 3 + col];
                if (col < 2) outFile << " ";
            }
            outFile << "\n";
        }
        outFile << "\n";
        
        // Write focal length
        outFile << std::fixed << std::setprecision(2) << camera_data[i].focal << "\n\n";
    }
}

} // namespace pano