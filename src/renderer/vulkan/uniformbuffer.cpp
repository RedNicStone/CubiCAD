//
// Created by nic on 25/09/2021.
//

#include "uniformbuffer.h"


UniformBuffer::~UniformBuffer() {
    buffer->unmap();
}

std::shared_ptr<UniformBuffer> UniformBuffer::create(const std::shared_ptr<Device> &pDevice,
                                                     const std::shared_ptr<Queue> &pTransferQueue,
                                                     size_t vSize) {
    auto uniformBuffer = std::make_shared<UniformBuffer>();

    uniformBuffer->buffer =
        Buffer::create(pDevice,
                       vSize,
                       VMA_MEMORY_USAGE_GPU_ONLY,
                       VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       {pTransferQueue->getQueueFamilyIndex()});
    uniformBuffer->size = vSize;
    uniformBuffer->device = pDevice;
    uniformBuffer->transferQueue = pTransferQueue;

    uniformBuffer->data = uniformBuffer->buffer->map();

    return uniformBuffer;
}
