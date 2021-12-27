//
// Created by nic on 18/07/2021.
//

#pragma once

#ifndef CUBICAD_PIPELINEBASE_H
#define CUBICAD_PIPELINEBASE_H

#include <vulkan/vulkan_core.h>

#include "device.h"
#include "vulkanclass.h"
#include "pipelinelayout.h"
#include "descriptorlayout.h"

#include <vector>


class PipelineBase : public VulkanClass<VkPipeline> {
  protected:
    std::shared_ptr<Device> device;

    std::shared_ptr<PipelineLayout> layout;

  public:
    std::shared_ptr<Device> getDevice() { return device; }

    std::shared_ptr<PipelineLayout> getLayout() { return layout; }

    virtual VkPipelineBindPoint getBindPoint() = 0;

    virtual ~PipelineBase() = default;
};

#endif //CUBICAD_PIPELINEBASE_H
