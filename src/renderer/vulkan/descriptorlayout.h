//
// Created by nic on 22/05/2021.
//

#pragma once

#ifndef CUBICAD_DESCRIPTORLAYOUT_H
#define CUBICAD_DESCRIPTORLAYOUT_H

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "vulkanclass.h"
#include "device.h"


class DescriptorSetLayout : public VulkanClass<VkDescriptorSetLayout> {
  private:
    Device *device;

  public:
    explicit DescriptorSetLayout(Device *device, std::vector<VkDescriptorSetLayoutBinding> &bindings_);

    ~DescriptorSetLayout();
};

#endif //CUBICAD_DESCRIPTORLAYOUT_H
