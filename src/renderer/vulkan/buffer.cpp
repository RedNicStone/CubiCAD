//
// Created by nic on 12/07/2021.
//

#include <vulkan/vulkan_core.h>

#include "buffer.h"


Buffer::Buffer(Device *device_,
               VkDeviceSize size,
               VmaMemoryUsage memoryUsage,
               VkMemoryPropertyFlags preferredFlags,
               VkBufferUsageFlagBits bufferUsage,
               std::vector<uint32_t> accessingQueues) {
    device = device_;
    deviceSize = size;

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = bufferUsage;

    if (accessingQueues.size() < 2)
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    else {
        createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
        createInfo.pQueueFamilyIndices = accessingQueues.data();
    }

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = memoryUsage;
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.preferredFlags = preferredFlags;

    if (vmaCreateBuffer(device->getAllocator(),
                        &createInfo,
                        &allocationCreateInfo,
                        &handle,
                        &allocation,
                        &allocationInfo) != VK_SUCCESS)
        throw std::runtime_error("could not allocate memory");
}

Buffer Buffer::createHostStagingBuffer(Device *device_, VkDeviceSize size, std::vector<uint32_t> &accessingQueues) {
    return Buffer(device_, size, VMA_MEMORY_USAGE_CPU_ONLY, 0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, accessingQueues);
}

void *Buffer::map() {
    if (allocationInfo.pMappedData != nullptr)
        return allocationInfo.pMappedData;

    void *data;
    vmaMapMemory(device->getAllocator(), allocation, &data);
    return data;
}

void Buffer::unmap() {
    if (allocationInfo.pMappedData == nullptr)
        vmaUnmapMemory(device->getAllocator(), allocation);
}

Buffer::~Buffer() {
    vmaDestroyBuffer(device->getAllocator(), handle, allocation);
}
