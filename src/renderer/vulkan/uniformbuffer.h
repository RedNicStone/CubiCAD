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


class Buffer;

class UniformBuffer {
  private:
    size_t size{};
    void *data{};

    std::shared_ptr<Buffer> buffer;

    std::shared_ptr<Device> device;
    std::shared_ptr<Queue> transferQueue;

  public:
    static std::shared_ptr<UniformBuffer> create(const std::shared_ptr<Device> &pDevice,
                                                 const std::shared_ptr<Queue> &pTransferQueue,
                                                 size_t vSize);

    void *getDataHandle() { return data; }

    [[nodiscard]] size_t getSize() const { return size; }

    std::shared_ptr<Buffer> getBuffer() { return buffer; }

    std::shared_ptr<Device> getDevice() { return device; }

    std::shared_ptr<Queue> getTransferQueue() { return transferQueue; }

    ~UniformBuffer();
};

#endif //CUBICAD_UNIFORMBUFFER_H
