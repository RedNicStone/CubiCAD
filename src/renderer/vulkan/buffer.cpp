//
// Created by nic on 12/07/2021.
//

#include <vulkan/vulkan_core.h>

#include <utility>

#include "buffer.h"


std::shared_ptr<Buffer> Buffer::create(std::shared_ptr<Device> pDevice,
                                       VkDeviceSize size,
                                       VmaMemoryUsage memoryUsage,
                                       VkBufferUsageFlags bufferUsage,
                                       std::vector<uint32_t> accessingQueues) {
    return Buffer::create(std::move(pDevice), size, memoryUsage, 0, 0, bufferUsage, std::move(accessingQueues));
}

std::shared_ptr<Buffer> Buffer::create(std::shared_ptr<Device> pDevice,
                                       VkDeviceSize size,
                                       VmaMemoryUsage memoryUsage,
                                       VkMemoryPropertyFlags preferredFlags,
                                       VkBufferUsageFlags bufferUsage,
                                       std::vector<uint32_t> accessingQueues) {
    return Buffer::create(std::move(pDevice), size, memoryUsage, preferredFlags, 0, bufferUsage, std::move(accessingQueues));
}

std::shared_ptr<Buffer> Buffer::create(std::shared_ptr<Device> pDevice,
                                       VkDeviceSize size,
                                       VmaMemoryUsage memoryUsage,
                                       VkMemoryPropertyFlags preferredFlags,
                                       VkMemoryPropertyFlags requiredFlags,
                                       VkBufferUsageFlags bufferUsage,
                                       std::vector<uint32_t> accessingQueues) {
    auto buffer = std::make_shared<Buffer>();
    buffer->device = std::move(pDevice);
    buffer->deviceSize = size;

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = bufferUsage;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
    createInfo.pQueueFamilyIndices = accessingQueues.data();

    if (accessingQueues.size() >= 2)
        createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    else {
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = memoryUsage;
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.preferredFlags = preferredFlags;
    allocationCreateInfo.requiredFlags = requiredFlags;

    if (vmaCreateBuffer(buffer->device->getAllocator(),
                        &createInfo,
                        &allocationCreateInfo,
                        &buffer->handle,
                        &buffer->allocation,
                        &buffer->allocationInfo) != VK_SUCCESS)
        throw std::runtime_error("could not allocate memory");

    return buffer;
}

std::shared_ptr<Buffer> Buffer::createHostStagingBuffer(std::shared_ptr<Device> pDevice,
                                                        VkDeviceSize size,
                                                        std::vector<uint32_t> &accessingQueues) {
    return Buffer::create(std::move(pDevice),
                          size,
                          VMA_MEMORY_USAGE_CPU_ONLY,
                          0,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          accessingQueues);
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

void Buffer::transferDataMapped(void *src) {
    void *dst;
    vmaMapMemory(device->getAllocator(), allocation, &dst);
    memcpy(dst, src, allocationInfo.size);
    vmaUnmapMemory(device->getAllocator(), allocation);
}

void Buffer::transferDataMapped(void *src, size_t size) {
    void *dst;
    vmaMapMemory(device->getAllocator(), allocation, &dst);
    memcpy(dst, src, size);
    vmaUnmapMemory(device->getAllocator(), allocation);
}

void Buffer::transferDataStaged(void *src, const std::shared_ptr<CommandPool>& commandPool) {
    std::vector<uint32_t> accessingQueues = { commandPool->getQueueFamily()->getQueueFamilyIndex() };
    auto stagingBuffer = Buffer::createHostStagingBuffer(device, allocationInfo.size, accessingQueues);
    stagingBuffer->transferDataMapped(src);

    auto commandBuffer = CommandBuffer::create(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    commandBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, shared_from_this(), { { 0, 0, allocationInfo.size } });
    commandBuffer->endCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer->getHandlePtr();

    commandPool->getQueue()->submitCommandBuffer({ submitInfo });
}
