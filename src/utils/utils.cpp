//
// Created by nic on 04/12/2021.
//

#include "utils.h"

#include <vector>
#include <string>
#include <fstream>
#include <boost/bimap.hpp>


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

uint_fast8_t Utils::getMSB(uint_fast64_t x)
{
    uint_fast8_t res = 0;
    while (x >>= 1)
        res++;
    return res - 1;
}

template <typename L, typename R>
boost::bimap<L, R> Utils::makeBimap(std::initializer_list<typename boost::bimap<L, R>::value_type> list)
{
    return boost::bimap<L, R>(list.begin(), list.end());
}
