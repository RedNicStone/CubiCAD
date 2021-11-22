//
// Created by nic on 08/03/2021.
//

#pragma once

#ifndef CUBICAD_DEVICE_H
#define CUBICAD_DEVICE_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <stdexcept>
#include <string>
#include <set>
#include <cstring>
#include <iostream>
#include <bitset>
#include <memory>

#include "physicaldevice.h"
#include "instance.h"


class PhysicalDevice;

class Device : public VulkanClass<VkDevice> {
  private:
    PhysicalDevice *physicalDevice;
    VmaAllocator allocator;

    void createMemoryAllocator();

  public:
    Device(PhysicalDevice *physicalDevice,
           std::vector<const char *> deviceExtensions,
           const VkPhysicalDeviceFeatures &deviceFeatures);

    PhysicalDevice &getPhysicalDevice() const { return *physicalDevice; }

    VmaAllocator getAllocator() { return allocator; }

    void cleanup();
};

#endif //CUBICAD_DEVICE_H
