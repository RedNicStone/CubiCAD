//
// Created by nic on 25/09/2021.
//

#pragma once

#ifndef CUBICAD_UNIFORMBUFFER_H
#define CUBICAD_UNIFORMBUFFER_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <utility>

#include "device.h"
#include "buffer.h"
#include "queues.h"


template<class type>
class UniformBuffer {
  private:
    type data;

    std::shared_ptr<Buffer> buffer;

    std::shared_ptr<Device> device;
    std::shared_ptr<Queue> transferQueue;

  public:
    static std::shared_ptr<UniformBuffer<type>> create(const std::shared_ptr<Device> &pDevice,
                                                 const std::shared_ptr<Queue> &pTransferQueue) {
        auto uniformBuffer = std::make_shared<UniformBuffer>();

        uniformBuffer->buffer =
            Buffer::create(pDevice,
                           sizeof(type),
                           VMA_MEMORY_USAGE_GPU_ONLY,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           { pTransferQueue->getQueueFamilyIndex() });
        uniformBuffer->device = pDevice;
        uniformBuffer->transferQueue = pTransferQueue;

        return uniformBuffer;
    }

    void updateBufferContents();

    type *getDataHandle() { return &data; }

    size_t getSize() { return sizeof(type); }

    std::shared_ptr<Buffer> getBuffer() { return buffer; }

    std::shared_ptr<Device> getDevice() { return device; }

    std::shared_ptr<Queue> getTransferQueue() { return transferQueue; }
};

#endif //CUBICAD_UNIFORMBUFFER_H
