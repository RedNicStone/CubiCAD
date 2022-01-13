//
// Created by nic on 16/07/2021.
//

#pragma once

#ifndef CUBICAD_BUFFERVIEW_H
#define CUBICAD_BUFFERVIEW_H

#include <vulkan/vulkan.h>

#include "buffer.h"

#include <iostream>


class BufferView : public VulkanClass<VkBufferView> {
  private:
    std::shared_ptr<Buffer> buffer;

  public:
    static std::shared_ptr<BufferView> create(const std::shared_ptr<Buffer> &pBuffer,
                                              VkFormat viewFormat,
                                              VkDeviceSize offset,
                                              VkDeviceSize range);
    static std::shared_ptr<BufferView> create(const std::shared_ptr<Buffer> &pBuffer, VkFormat viewFormat);

    std::shared_ptr<Buffer> getBuffer() { return buffer; }

    ~BufferView();
};

#endif //CUBICAD_BUFFERVIEW_H
