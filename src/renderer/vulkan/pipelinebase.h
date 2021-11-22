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
    Device *device;

    PipelineLayout *layout;

    PipelineBase(Device *device, PipelineLayout *layout) : device(device), layout(layout) {};

  public:
    Device &getDevice() { return *device; }

    PipelineLayout *getLayout() { return layout; }

    virtual VkPipelineBindPoint getBindPoint() = 0;
};

#endif //CUBICAD_PIPELINEBASE_H
