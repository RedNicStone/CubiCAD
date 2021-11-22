//
// Created by nic on 08/03/2021.
//

#include "types.h"


bool QueueFamilyIndices::isComplete() const {
    return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
}

std::vector<uint32_t> QueueFamilyIndices::getUniqueQueueFamilies() {
    std::vector<uint32_t> vector(3);

    if (graphicsFamily.has_value()) vector.push_back(graphicsFamily.value());
    if (presentFamily.has_value()) vector.push_back(presentFamily.value());
    if (transferFamily.has_value()) vector.push_back(transferFamily.value());

    return vector;
}
