//
// Created by nic on 21/05/2021.
//

#ifndef CUBICAD_UTILS_H
#define CUBICAD_UTILS_H

#include <vector>
#include <string>
#include <fstream>


static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

#endif //CUBICAD_UTILS_H
