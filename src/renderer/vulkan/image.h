//
// Created by nic on 16/07/2021.
//

#pragma once

#ifndef CUBICAD_IMAGE_H
#define CUBICAD_IMAGE_H

#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>

#include "device.h"
#include "vulkanclass.h"
#include "queues.h"
#include "imageview.h"

#include <vector>


class ImageView;

class Image : public VulkanClass<VkImage> {
  private:
    Device *device;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo{};

    VkExtent3D extent;
    VkImageType type;
    VkFormat format;
    uint32_t mipLevels;
    uint32_t arrayLayers;

  public:
    Image(Device *device_,
          VmaMemoryUsage memoryUsage,
          VkMemoryPropertyFlags preferredFlags,
          VkImageCreateInfo &createInfo,
          std::vector<uint32_t> &accessingQueues);

    Image(Device *device_,
          const VkExtent2D &size,
          uint32_t mip_level,
          VkFormat format,
          VkImageUsageFlags usage,
          std::vector<uint32_t> &accessingQueues);

    ImageView createImageView(VkImageViewType viewType, VkImageSubresourceRange subresourceRange);

    Device &getDevice() const { return *device; }

    const VkExtent3D &getExtent() const { return extent; }

    VkImageType getType() const { return type; }

    VkFormat getFormat() const { return format; }

    uint32_t getMipLevels() const { return mipLevels; }

    uint32_t getArrayLayers() const { return arrayLayers; }

    ~Image();
};

#endif //CUBICAD_IMAGE_H
