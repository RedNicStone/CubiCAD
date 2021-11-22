//
// Created by nic on 20/04/2021.
//

#include "imageview.h"


ImageView::ImageView(Image *pImage, VkImageViewType viewType, VkImageSubresourceRange &subresourceRange) {
    image = pImage;

    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image->getHandle();
    createInfo.viewType = viewType;
    createInfo.format = image->getFormat();
    createInfo.subresourceRange = subresourceRange;

    if (vkCreateImageView(image->getDevice().getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance image view!");
    }
}

ImageView::ImageView(Image *pImage, VkImageViewType viewType, VkImageAspectFlags aspectFlags) {
    image = pImage;

    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image->getHandle();
    createInfo.viewType = viewType;
    createInfo.format = image->getFormat();
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = pImage->getMipLevels();
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = pImage->getArrayLayers();

    if (vkCreateImageView(image->getDevice().getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance image view!");
    }
}

ImageView::~ImageView() {
    vkDestroyImageView(image->getDevice().getHandle(), handle, nullptr);
}
