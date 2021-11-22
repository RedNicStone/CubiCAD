//
// Created by nic on 25/09/2021.
//

#pragma once

#ifndef CUBICAD_UNIFORMBUFFER_H
#define CUBICAD_UNIFORMBUFFER_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "device.h"
#include "buffer.h"
#include "queues.h"

template<class type>
class UniformBuffer {
  private:
    type data;

    Buffer buffer;

    Device* device;
    Queue* transferQueue;

  public:
    UniformBuffer(Device* pDevice, Queue* pTransferQueue) : buffer(pDevice, sizeof(type),
                                                                   VMA_MEMORY_USAGE_GPU_ONLY,
                                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                   { pTransferQueue->getQueueFamilyIndex() }){
        device = pDevice;
        transferQueue = pTransferQueue;
    }

    void updateBufferContents();

    type* getDataHandle() { return &data; }

    Buffer* getBuffer() { return &buffer; }
    Device* getDevice() { return device; }
    Queue* getTransferQueue() { return transferQueue; }
};

#endif //CUBICAD_UNIFORMBUFFER_H
