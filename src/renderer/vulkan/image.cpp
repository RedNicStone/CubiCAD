//
// Created by nic on 16/07/2021.
//

#include <vulkan/vulkan_core.h>

#include <utility>

#include "image.h"


std::shared_ptr<Image> Image::create(std::shared_ptr<Device> pDevice,
                                     VmaMemoryUsage memoryUsage,
                                     VkMemoryPropertyFlags preferredFlags,
                                     VkMemoryPropertyFlags requiredFlags,
                                     VkImageCreateInfo &createInfo,
                                     std::vector<uint32_t> &accessingQueues) {
    auto image = std::shared_ptr<Image>();
    image->device = std::move(pDevice);

    image->extent = createInfo.extent;
    image->type = createInfo.imageType;
    image->format = createInfo.format;
    image->mipLevels = createInfo.mipLevels;
    image->arrayLayers = createInfo.arrayLayers;

    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
    createInfo.pQueueFamilyIndices = accessingQueues.data();

    if (accessingQueues.size() >= 2)
        createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    else {
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = memoryUsage;
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.preferredFlags = preferredFlags;
    allocationCreateInfo.requiredFlags = requiredFlags;

    if (vmaCreateImage(image->device->getAllocator(),
                       &createInfo,
                       &allocationCreateInfo,
                       &image->handle,
                       &image->allocation,
                       &image->allocationInfo) != VK_SUCCESS)
        throw std::runtime_error("could not allocate memory");
}

std::shared_ptr<Image> Image::create(const std::shared_ptr<Device>& pDevice,
                                     const VkExtent2D &size,
                                     uint32_t mip_level,
                                     VkFormat format,
                                     VkImageUsageFlags usage,
                                     std::vector<uint32_t> &accessingQueues) {
    auto image = std::make_shared<Image>();
    image->device = pDevice;

    image->extent = {size.width, size.height, 0};
    image->type = VK_IMAGE_TYPE_2D;
    image->format = format;
    image->mipLevels = mip_level;
    image->arrayLayers = 1;

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent = {size.width, size.height, 1};
    createInfo.mipLevels = mip_level;
    createInfo.arrayLayers = 1;
    createInfo.format = format;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
    createInfo.pQueueFamilyIndices = accessingQueues.data();
    createInfo.flags = 0;

    if (accessingQueues.size() >= 2)
        createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    else {
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.flags = 0;
    allocationCreateInfo.preferredFlags = 0;

    if (vmaCreateImage(image->device->getAllocator(),
                       &createInfo,
                       &allocationCreateInfo,
                       &image->handle,
                       &image->allocation,
                       &image->allocationInfo) != VK_SUCCESS)
        throw std::runtime_error("could not allocate memory");

    return image;
}

std::shared_ptr<ImageView> Image::createImageView(VkImageViewType viewType, VkImageSubresourceRange subresourceRange) {
    return ImageView::create(shared_from_this(), viewType, subresourceRange);
}

Image::~Image() {
    vmaDestroyImage(device->getAllocator(), handle, allocation);
}
