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
    std::shared_ptr<PhysicalDevice> physicalDevice;
    VmaAllocator allocator;

    void createMemoryAllocator();

  public:
    static std::shared_ptr<Device> create(const std::shared_ptr<PhysicalDevice> &physicalDevice,
                                          std::vector<const char *> deviceExtensions,
                                          const VkPhysicalDeviceFeatures &deviceFeatures);

    void waitIdle();

    std::shared_ptr<PhysicalDevice> getPhysicalDevice() { return physicalDevice; }

    VmaAllocator getAllocator() { return allocator; }

    ~Device();
};

#endif //CUBICAD_DEVICE_H
