//
// Created by nic on 16/07/2021.
//

#include "bufferview.h"


BufferView::BufferView(Buffer *pBuffer, VkFormat viewFormat, VkDeviceSize offset, VkDeviceSize range) {
    buffer = pBuffer;

    VkBufferViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    createInfo.buffer = pBuffer->getHandle();
    createInfo.format = viewFormat;
    createInfo.offset = offset;
    createInfo.range = range;

    vkCreateBufferView(pBuffer->getDevice().getHandle(), &createInfo, nullptr, &handle);
}

BufferView::BufferView(Buffer *pBuffer, VkFormat viewFormat) : BufferView(pBuffer, viewFormat, 0, pBuffer->getSize()) {}

BufferView::~BufferView() {
    vkDestroyBufferView(buffer->getDevice().getHandle(), handle, nullptr);
}
