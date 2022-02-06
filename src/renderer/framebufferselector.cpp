//
// Created by nic on 20/01/2022.
//

#include "framebufferselector.h"


std::shared_ptr<FramebufferSelector> FramebufferSelector::create(const std::shared_ptr<Device> &device,
                                                                 const std::shared_ptr<Queue> &renderQueue,
                                                                 VkExtent2D imageExtent) {
    auto objectSelector = std::make_shared<FramebufferSelector>();

    std::vector<uint32_t> accessingQueues{renderQueue->getQueueFamilyIndex()};

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent = {imageExtent.width, imageExtent.height, 1};
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = VK_FORMAT_R32_UINT;
    createInfo.tiling = VK_IMAGE_TILING_LINEAR;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
    createInfo.pQueueFamilyIndices = accessingQueues.data();
    createInfo.flags = 0;

    objectSelector->image =
        Image::create(device,
                      VMA_MEMORY_USAGE_UNKNOWN,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      createInfo,
                      accessingQueues);

    objectSelector->data = static_cast<uint32_t *>(objectSelector->image->map());
    objectSelector->extent = imageExtent;

    VkImageSubresource imageSubresource{};
    imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageSubresource.arrayLayer = 0;
    imageSubresource.mipLevel = 0;

    vkGetImageSubresourceLayout(device->getHandle(),
                                objectSelector->image->getHandle(),
                                &imageSubresource,
                                &(objectSelector->imageLayout));
    objectSelector->imageLayout.rowPitch %= imageExtent.width;
    objectSelector->imageLayout.rowPitch += imageExtent.width;

    return objectSelector;
}

void FramebufferSelector::transferLayoutRead(const std::shared_ptr<CommandBuffer> &commandBuffer) {
    image->transitionImageLayout(commandBuffer,
                                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                 VK_PIPELINE_STAGE_HOST_BIT,
                                 VK_ACCESS_HOST_READ_BIT);
}

void FramebufferSelector::transferLayoutWrite(const std::shared_ptr<CommandBuffer> &commandBuffer) {
    image->transitionImageLayout(commandBuffer,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_ACCESS_TRANSFER_WRITE_BIT);
}

uint32_t FramebufferSelector::getIDAtPosition(VkExtent2D position) {
    if (position.width > 0 && position.height > 0 && position.width < extent.width && position.height < extent.height) {
        return *(data + position.height * imageLayout.rowPitch + position.width);
    }
    return 0;
}

FramebufferSelector::~FramebufferSelector() {
    if (image != nullptr)
        image->unmap();
}
