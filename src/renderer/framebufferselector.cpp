//
// Created by nic on 20/01/2022.
//

#include "framebufferselector.h"


std::shared_ptr<FramebufferSelector> FramebufferSelector::create(const std::shared_ptr<Device>& device,
                                                                 const std::shared_ptr<Queue>& renderQueue,
                                                                 VkExtent2D extent) {
    auto objectSelector = std::make_shared<FramebufferSelector>();

    std::vector<uint32_t> accessingQueues{ renderQueue->getQueueFamilyIndex() };

    /*VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
    imageCreateInfo.extent = { extent.width, extent.height, 1};
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_R32_UINT;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    objectSelector->image = Image::create(device, VMA_MEMORY_USAGE_GPU_TO_CPU, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, imageCreateInfo, accessingQueues);*/

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent = { extent.width, extent.height, 1};
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = VK_FORMAT_R32_UINT;
    createInfo.tiling = VK_IMAGE_TILING_LINEAR;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
    createInfo.pQueueFamilyIndices = accessingQueues.data();
    createInfo.flags = 0;

    objectSelector->image = Image::create(device, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          0, createInfo, accessingQueues);

    objectSelector->imageView = ImageView::create(objectSelector->image, VK_IMAGE_VIEW_TYPE_2D,
                                                  VK_IMAGE_ASPECT_COLOR_BIT);

    objectSelector->data = static_cast<uint32_t*>(objectSelector->image->map());

    return objectSelector;
}

uint32_t FramebufferSelector::getIDAtPosition(VkExtent2D position) {
    //return *(data + position.height * image->getExtent().width + position.width);
}

FramebufferSelector::~FramebufferSelector() {
    if (image != nullptr)
    image->unmap();
}
