//
// Created by nic on 16/07/2021.
//

#include <vulkan/vulkan_core.h>

#include "image.h"


Image::Image(Device *device_,
             VmaMemoryUsage memoryUsage,
             VkMemoryPropertyFlags preferredFlags,
             VkImageCreateInfo &createInfo,
             std::vector<uint32_t> &accessingQueues) {
    device = device_;

    extent = createInfo.extent;
    type = createInfo.imageType;
    format = createInfo.format;
    mipLevels = createInfo.mipLevels;
    arrayLayers = createInfo.arrayLayers;

    if (accessingQueues.size() >= 2)
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    else {
        createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
        createInfo.pQueueFamilyIndices = accessingQueues.data();
    }

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = memoryUsage;
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.preferredFlags = preferredFlags;

    if (vmaCreateImage(device->getAllocator(),
                       &createInfo,
                       &allocationCreateInfo,
                       &handle,
                       &allocation,
                       &allocationInfo) != VK_SUCCESS)
        throw std::runtime_error("could not allocate memory");
}

Image::Image(Device *device_,
             const VkExtent2D &size,
             uint32_t mip_level,
             VkFormat format_,
             VkImageUsageFlags usage,
             std::vector<uint32_t> &accessingQueues) {
    extent = {size.width, size.height, 0};
    type = VK_IMAGE_TYPE_2D;
    format = format_;
    mipLevels = mip_level;
    arrayLayers = 1;

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent = {size.width, size.height, 0};
    createInfo.mipLevels = mip_level;
    createInfo.arrayLayers = 1;
    createInfo.format = format_;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    device = device_;

    if (accessingQueues.size() >= 2)
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    else {
        createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
        createInfo.pQueueFamilyIndices = accessingQueues.data();
    }

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationCreateInfo.preferredFlags = 0;

    if (vmaCreateImage(device->getAllocator(),
                       &createInfo,
                       &allocationCreateInfo,
                       &handle,
                       &allocation,
                       &allocationInfo) != VK_SUCCESS)
        throw std::runtime_error("could not allocate memory");
}

ImageView Image::createImageView(VkImageViewType viewType, VkImageSubresourceRange subresourceRange) {
    return ImageView(this, viewType, subresourceRange);
}

Image::~Image() {
    vmaDestroyImage(device->getAllocator(), handle, allocation);
}
