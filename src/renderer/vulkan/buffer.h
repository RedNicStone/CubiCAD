//
// Created by nic on 12/07/2021.
//

#pragma once

#ifndef CUBICAD_BUFFER_H
#define CUBICAD_BUFFER_H

#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>

#include "device.h"
#include "vulkanclass.h"
#include "queues.h"

#include <vector>


class Buffer : public VulkanClass<VkBuffer> {
  private:
    Device *device;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo{};

    VkDeviceSize deviceSize;

  public:
    Buffer(Device *device_,
           VkDeviceSize size,
           VmaMemoryUsage memoryUsage,
           VkMemoryPropertyFlags preferredFlags,
           VkBufferUsageFlagBits bufferUsage,
           std::vector<uint32_t> accessingQueues);

    static Buffer createHostStagingBuffer(Device *device_, VkDeviceSize size, std::vector<uint32_t> &accessingQueues);

    Device &getDevice() const { return *device; }

    VkDeviceSize getSize() const { return deviceSize; }

    void *map();
    void unmap();

    ~Buffer();  // TODO is this object destructed when it goes out of scope?
};

#endif //CUBICAD_BUFFER_H
