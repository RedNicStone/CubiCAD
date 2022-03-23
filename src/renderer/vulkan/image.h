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
#include "commandpool.h"
#include "buffer.h"

#include <vector>


class ImageView;

class CommandBuffer;

class Buffer;

class Image : public VulkanClass<VkImage>, public std::enable_shared_from_this<Image> {
  private:
    std::shared_ptr<Device> device;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo{};

    VkExtent3D extent;
    VkImageType type;
    VkFormat format;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkAccessFlags currentAccessFlags = 0;
    VkPipelineStageFlags currentStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

  public:
    static std::shared_ptr<Image> create(const std::shared_ptr<Device> &pDevice,
                                         VmaMemoryUsage memoryUsage,
                                         VkMemoryPropertyFlags preferredFlags,
                                         VkMemoryPropertyFlags requiredFlags,
                                         VkImageCreateInfo &createInfo,
                                         std::vector<uint32_t> &accessingQueues,
                                         VkAccessFlags initialAccessMask = 0,
                                         VkPipelineStageFlags initialPipelineStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    static std::shared_ptr<Image> create(const std::shared_ptr<Device> &pDevice,
                                         const VkExtent2D &size,
                                         uint32_t mip_level,
                                         VkFormat format,
                                         VkImageUsageFlags usage,
                                         std::vector<uint32_t> &accessingQueues,
                                         VkAccessFlags initialAccessMask = 0,
                                         VkPipelineStageFlags initialPipelineStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    static std::shared_ptr<Image> create(const std::shared_ptr<Device> &pDevice,
                                         const VkExtent2D &size,
                                         uint32_t mip_level,
                                         VkFormat format,
                                         VmaMemoryUsage memoryUsage,
                                         VkImageUsageFlags usage,
                                         std::vector<uint32_t> &accessingQueues,
                                         VkAccessFlags initialAccessMask = 0,
                                         VkPipelineStageFlags initialPipelineStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    static std::shared_ptr<Image> createHostStagingImage(const std::shared_ptr<Device> &pDevice,
                                                         const VkExtent2D &size,
                                                         VkFormat format,
                                                         std::vector<uint32_t> &accessingQueues);

    static std::shared_ptr<Buffer> createHostStagingBuffer(const std::shared_ptr<Device> &pDevice,
                                                           VkDeviceSize size,
                                                           std::vector<uint32_t> &accessingQueues);

    void transferDataMapped(void *src);
    void transferDataMapped(void *src, size_t size);
    void transferDataStaged(void *src, const std::shared_ptr<CommandPool> &commandPool);
    void transferDataStaged(void *src,
                            const std::shared_ptr<CommandPool> &commandPool,
                            VkDeviceSize size, [[maybe_unused]] VkDeviceSize offset = 0,
                            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

    void generateMipmaps(const std::shared_ptr<CommandPool> &commandPool);

    std::shared_ptr<ImageView> createImageView(VkImageViewType viewType, VkImageSubresourceRange subresourceRange);

    std::shared_ptr<Device> getDevice() { return device; }

    [[nodiscard]] const VkExtent3D &getExtent() const { return extent; }

    [[nodiscard]] VkImageType getType() const { return type; }

    [[nodiscard]] VkFormat getFormat() const { return format; }

    [[nodiscard]] uint32_t getMipLevels() const { return mipLevels; }

    [[nodiscard]] uint32_t getArrayLayers() const { return arrayLayers; }

    [[nodiscard]] VkImageLayout getLayout() const { return currentLayout; }

    void setLayout(VkImageLayout layout) { currentLayout = layout; }

    void transitionImageLayout(const std::shared_ptr<CommandPool> &commandPool, VkImageLayout dst);

    void transitionImageLayout(const std::shared_ptr<CommandBuffer> &commandPool,
                               VkImageLayout dst,
                               VkPipelineStageFlags dstStage,
                               VkAccessFlags dstMask = 0,
                               VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT);

    void *map();
    void unmap();

    ~Image();
};

#endif //CUBICAD_IMAGE_H
