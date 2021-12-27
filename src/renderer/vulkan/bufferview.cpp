//
// Created by nic on 16/07/2021.
//

#include "bufferview.h"


std::shared_ptr<BufferView> BufferView::create(const std::shared_ptr<Buffer> &pBuffer,
                                               VkFormat viewFormat,
                                               VkDeviceSize offset,
                                               VkDeviceSize range) {
    auto bufferView = std::make_shared<BufferView>();
    bufferView->buffer = pBuffer;

    VkBufferViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    createInfo.buffer = pBuffer->getHandle();
    createInfo.format = viewFormat;
    createInfo.offset = offset;
    createInfo.range = range;

    vkCreateBufferView(pBuffer->getDevice()->getHandle(), &createInfo, nullptr, &bufferView->handle);

    return bufferView;
}

std::shared_ptr<BufferView> BufferView::create(const std::shared_ptr<Buffer> &pBuffer, VkFormat viewFormat) {
    return BufferView::create(pBuffer, viewFormat, 0, pBuffer->getSize());
}

BufferView::~BufferView() {
    vkDestroyBufferView(buffer->getDevice()->getHandle(), handle, nullptr);
}
