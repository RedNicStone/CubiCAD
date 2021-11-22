//
// Created by nic on 20/09/2021.
//

#pragma once

#ifndef CUBICAD_FENCE_H
#define CUBICAD_FENCE_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "vulkanclass.h"
#include "device.h"

class Device;

class Fence : public VulkanClass<VkFence> {
  private:
    Device* device;

  public:
    explicit Fence(Device* pDevice, bool signaled = false);

    bool getState();
    void resetState();
    void waitForSignal(uint64_t timeout = UINT64_MAX);

    ~Fence();
};

#endif //CUBICAD_FENCE_H
