//
// Created by nic on 04/12/2021.
//

#include "utils.h"

#include <string>
#include <fstream>


std::vector<char> Utils::readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<long>(fileSize));

    file.close();

    return buffer;
}

uint_fast8_t Utils::getMSB(uint_fast64_t x) {
    uint_fast8_t res = 0;
    while (x >>= 1)
        res++;
    return res - 1;
}

void Utils::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}
