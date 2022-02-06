//
// Created by nic on 25/01/2022.
//

#pragma once

#ifndef CUBICAD_SAMPLER_H
#define CUBICAD_SAMPLER_H

#include <vulkan/vulkan_core.h>

#include "device.h"
#include "vulkanclass.h"


class Sampler : public VulkanClass<VkSampler> {
  private:
    std::shared_ptr<Device> device;

  public:
    static std::shared_ptr<Sampler> create(const std::shared_ptr<Device> &pDevice, float anisotropy);

    ~Sampler();
};

#endif //CUBICAD_SAMPLER_H
