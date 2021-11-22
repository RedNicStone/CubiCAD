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
    Device *device;
    Queue *queue;

  public:
    CommandPool(Device *pDevice, Queue *pQueue, VkCommandPoolCreateFlags flags);

    Queue *getQueue() { return queue; }

    Device *getDevice() { return device; }

    QueueFamily *getQueueFamily() { return queue->getQueueFamily(); }

    ~CommandPool();
};

#endif //CUBICAD_SRC_RENDERER_VULKAN_COMMANDPOOL_H_
