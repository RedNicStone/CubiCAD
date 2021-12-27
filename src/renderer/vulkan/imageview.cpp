//
// Created by nic on 20/04/2021.
//

#include "imageview.h"

#include <utility>


std::shared_ptr<ImageView> ImageView::create(std::shared_ptr<Image> pImage,
                                             VkImageViewType viewType,
                                             VkImageSubresourceRange &subresourceRange) {
    auto imageView = std::make_shared<ImageView>();
    imageView->image = std::move(pImage);

    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = imageView->image->getHandle();
    createInfo.viewType = viewType;
    createInfo.format = imageView->image->getFormat();
    createInfo.subresourceRange = subresourceRange;

    if (vkCreateImageView(imageView->image->getDevice()->getHandle(), &createInfo, nullptr, &imageView->handle)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance image view!");
    }

    return imageView;
}

std::shared_ptr<ImageView> ImageView::create(std::shared_ptr<Image> pImage,
                                             VkImageViewType viewType,
                                             VkImageAspectFlags aspectFlags) {
    auto imageView = std::make_shared<ImageView>();
    imageView->image = std::move(pImage);

    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = imageView->image->getHandle();
    createInfo.viewType = viewType;
    createInfo.format = imageView->image->getFormat();
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = imageView->image->getMipLevels();
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = imageView->image->getArrayLayers();

    if (vkCreateImageView(imageView->image->getDevice()->getHandle(), &createInfo, nullptr, &imageView->handle)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance image view!");
    }

    return imageView;
}

ImageView::~ImageView() {
    vkDestroyImageView(image->getDevice()->getHandle(), handle, nullptr);
}
