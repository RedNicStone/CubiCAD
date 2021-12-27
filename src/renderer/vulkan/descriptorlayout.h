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
    std::shared_ptr<Device> device;

  public:
    static std::shared_ptr<DescriptorSetLayout> create(std::shared_ptr<Device> pDevice,
                                                       std::vector<VkDescriptorSetLayoutBinding> &bindings);

    ~DescriptorSetLayout();
};

#endif //CUBICAD_DESCRIPTORLAYOUT_H
