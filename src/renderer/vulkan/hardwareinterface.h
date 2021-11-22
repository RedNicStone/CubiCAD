//
// Created by nic on 08/03/2021.
//

#ifndef CUBICAD_HARDWAREINTERFACE_H
#define CUBICAD_HARDWAREINTERFACE_H

#include <vulkan/vulkan.h>

#include <vector>


class HardwareInterface {
    std::vector<VkPhysicalDevice> listPhysicalDevices();
};

#endif //CUBICAD_HARDWAREINTERFACE_H
