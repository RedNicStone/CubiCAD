//
// Created by nic on 20/01/2022.
//

#include "framebufferselector.h"


std::shared_ptr<FramebufferSelector> FramebufferSelector::create(const std::shared_ptr<Device>& device,
                                                                 const std::shared_ptr<Queue>& renderQueue,
                                                                 VkExtent2D imageExtent) {
    auto objectSelector = std::make_shared<FramebufferSelector>();

    std::vector<uint32_t> accessingQueues{ renderQueue->getQueueFamilyIndex() };
    objectSelector->image = Image::createHostStagingImage(device, imageExtent, VK_FORMAT_R32_UINT, accessingQueues);

    objectSelector->data = static_cast<uint32_t*>(objectSelector->image->map());
    objectSelector->extent = imageExtent;

    VkImageSubresource imageSubresource{};
    imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageSubresource.arrayLayer = 0;
    imageSubresource.mipLevel = 0;

    vkGetImageSubresourceLayout(device->getHandle(), objectSelector->image->getHandle(), &imageSubresource,
                                &(objectSelector->imageLayout));
    objectSelector->imageLayout.rowPitch %= imageExtent.width;
    objectSelector->imageLayout.rowPitch += imageExtent.width;

    return objectSelector;
}

void FramebufferSelector::transferLayoutRead(const std::shared_ptr<CommandBuffer>& commandBuffer) {
    image->transitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_HOST_BIT,
    VK_ACCESS_HOST_READ_BIT);
}

void FramebufferSelector::transferLayoutWrite(const std::shared_ptr<CommandBuffer>& commandBuffer) {
    image->transitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT,
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
