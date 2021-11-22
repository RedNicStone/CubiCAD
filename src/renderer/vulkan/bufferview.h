//
// Created by nic on 16/07/2021.
//

#ifndef CUBICAD_BUFFERVIEW_H
#define CUBICAD_BUFFERVIEW_H

#include <vulkan/vulkan.h>

#include "buffer.h"

#include <iostream>


class BufferView : public VulkanClass<VkBufferView> {
  private:
    Buffer *buffer;

  public:
    BufferView(Buffer *pBuffer, VkFormat viewFormat, VkDeviceSize offset, VkDeviceSize range);
    BufferView(Buffer *pBuffer, VkFormat viewFormat);

    Buffer &getBuffer() { return *buffer; }

    ~BufferView();
};

#endif //CUBICAD_BUFFERVIEW_H
