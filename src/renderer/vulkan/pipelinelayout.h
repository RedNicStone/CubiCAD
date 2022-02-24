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
    std::shared_ptr<Device> device;

    std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;
    std::vector<std::shared_ptr<PushConstantRange>> pushConstantRanges;

  public:
    static std::shared_ptr<PipelineLayout> create(const std::shared_ptr<Device>& pDevice,
                                                  const std::vector<std::shared_ptr<DescriptorSetLayout>>
                                                  &descriptorLayouts,
                                                  const std::vector<std::shared_ptr<PushConstantRange>> &pushConstants);

    static std::shared_ptr<PipelineLayout> create(const std::shared_ptr<Device>& pDevice,
                                                  const std::vector<std::shared_ptr<DescriptorSetLayout>>
                                                  &descriptorLayouts);

    static std::shared_ptr<PipelineLayout> create(const std::shared_ptr<Device>& pDevice,
                                                  const std::vector<std::shared_ptr<PushConstantRange>> &pushConstants);

    static std::shared_ptr<PipelineLayout> create(std::shared_ptr<Device> pDevice);

    std::shared_ptr<Device> getDevice() { return device; }

    ~PipelineLayout();
};

#endif //CUBICAD_PIPELINELAYOUT_H
