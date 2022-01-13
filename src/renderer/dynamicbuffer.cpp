//
// Created by nic on 13/01/2022.
//

#include "dynamicbuffer.h"

#include <utility>


std::shared_ptr<DynamicBuffer> DynamicBuffer::create(std::shared_ptr<Device> pDevice,
                                              const std::vector<uint32_t>& vAccessingQueues,
                                              VmaMemoryUsage vMemoryUsage,
                                              VkBufferUsageFlags vBufferUsage,
                                              VkMemoryPropertyFlags vPreferredFlags,
                                              VkMemoryPropertyFlags vRequiredFlags) {
    auto buffer = std::make_shared<DynamicBuffer>();

    buffer->device = std::move(pDevice);
    buffer->accessingQueues = vAccessingQueues;
    buffer->memoryUsage = vMemoryUsage;
    buffer->bufferUsage = vBufferUsage;
    buffer->preferredFlags = vPreferredFlags;
    buffer->requiredFlags = vRequiredFlags;

    return buffer;
}

std::shared_ptr<Buffer> DynamicBuffer::getBuffer(VkDeviceSize size) {
    if (buffer == nullptr || size > buffer->getSize()) {
        if (mapped && buffer != nullptr)
            buffer->unmap();
        buffer = Buffer::create(device, size, memoryUsage, preferredFlags, requiredFlags, bufferUsage |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                accessingQueues);
        if (mapped)
            data = buffer->map();
    }
    return buffer;
}

std::shared_ptr<Buffer> DynamicBuffer::getBufferPreserveContents(VkDeviceSize size, const std::shared_ptr<CommandPool>&
commandPool) {
    if (buffer == nullptr) {
        buffer = Buffer::create(device, size, memoryUsage, preferredFlags, requiredFlags, bufferUsage, accessingQueues);
    }
    if (size > buffer->getSize()) {
        if (mapped)
            buffer->unmap();
        auto newBuffer = Buffer::create(device, size, memoryUsage, preferredFlags, requiredFlags, bufferUsage, accessingQueues);

        auto commandBuffer = CommandBuffer::create(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        commandBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        commandBuffer->copyBuffer(buffer, newBuffer, { { 0, 0, buffer->getSize() } });
        commandBuffer->endCommandBuffer();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer->getHandlePtr();

        commandPool->getQueue()->submitCommandBuffer({ submitInfo });

        buffer = newBuffer;
        if (mapped)
            data = buffer->map();
    }
    return buffer;
}

void **DynamicBuffer::map() {
    if (buffer != nullptr) {
        data = buffer->map();
    }
    mapped = true;
    return &data;
}

void DynamicBuffer::unmap() {
    mapped = false;
    buffer->unmap();
}
