//
// Created by nic on 03/07/2021.
//

#pragma once

#ifndef CUBICAD_SRC_RENDERER_VULKAN_COMMANDPOOL_H_
#define CUBICAD_SRC_RENDERER_VULKAN_COMMANDPOOL_H_

#include <vulkan/vulkan_core.h>

#include "vulkanclass.h"
#include "queues.h"


class CommandPool : public VulkanClass<VkCommandPool> {
  private:
    std::shared_ptr<Device> device;
    std::shared_ptr<Queue> queue;

  public:
    static std::shared_ptr<CommandPool> create(const std::shared_ptr<Device> &pDevice,
                                               const std::shared_ptr<Queue> &pQueue,
                                               VkCommandPoolCreateFlags flags);

    void reset();

    std::shared_ptr<Queue> getQueue() { return queue; }

    std::shared_ptr<Device> getDevice() { return device; }

    std::shared_ptr<QueueFamily> getQueueFamily() { return queue->getQueueFamily(); }

    ~CommandPool();
};

#endif //CUBICAD_SRC_RENDERER_VULKAN_COMMANDPOOL_H_
