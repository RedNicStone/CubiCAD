//
// Created by nic on 22/05/2021.
//

#pragma once

#ifndef CUBICAD_PIPELINELAYOUT_H
#define CUBICAD_PIPELINELAYOUT_H

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>
#include <functional>

#include "descriptorlayout.h"
#include "pushconstant.h"
#include "device.h"
#include "vulkanclass.h"


class PipelineLayout : public VulkanClass<VkPipelineLayout> {
  private:
    Device *device;

    std::vector<DescriptorSetLayout> descriptorSetLayouts;
    std::vector<PushConstantRange> pushConstantRanges;

  public:
    PipelineLayout(Device *pDevice,
                   std::vector<DescriptorSetLayout> &descriptorLayouts,
                   std::vector<PushConstantRange> &pushConstants);

    PipelineLayout(Device *pDevice, std::vector<DescriptorSetLayout> &descriptorLayouts);

    PipelineLayout(Device *pDevice, std::vector<PushConstantRange> &pushConstants);

    explicit PipelineLayout(Device *pDevice);

    Device &getDevice() const { return *device; }

    ~PipelineLayout();
};

#endif //CUBICAD_PIPELINELAYOUT_H
