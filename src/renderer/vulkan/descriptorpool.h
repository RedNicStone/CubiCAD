//
// Created by nic on 23/08/2021.
//

#pragma once

#ifndef CUBICAD_DESCRIPTORPOOL_H
#define CUBICAD_DESCRIPTORPOOL_H

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "vulkanclass.h"
#include "device.h"


class DescriptorPool : public VulkanClass<VkDescriptorPool> {
  private:
    std::shared_ptr<Device> device;

  public:
    static std::shared_ptr<DescriptorPool> create(std::shared_ptr<Device> pDevice,
                                                  std::vector<VkDescriptorPoolSize> &sizes,
                                                  uint32_t maxSets = 0);

    std::shared_ptr<Device> getDevice() { return device; }

    ~DescriptorPool();
};

#endif //CUBICAD_DESCRIPTORPOOL_H
