//
// Created by nic on 16/07/2021.
//

#include <vulkan/vulkan_core.h>

#include <utility>

#include "image.h"


std::shared_ptr<Image> Image::create(const std::shared_ptr<Device>& pDevice,
                                     VmaMemoryUsage memoryUsage,
                                     VkMemoryPropertyFlags preferredFlags,
                                     VkMemoryPropertyFlags requiredFlags,
                                     VkImageCreateInfo &createInfo,
                                     std::vector<uint32_t> &accessingQueues,
                                     VkAccessFlags initialAccessMask,
                                     VkPipelineStageFlags initialPipelineStage) {
    auto image = std::make_shared<Image>();
    image->device = pDevice;
    image->currentAccessFlags = initialAccessMask;
    image->currentStage = initialPipelineStage;

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

    return image;
}

std::shared_ptr<Image> Image::create(const std::shared_ptr<Device>& pDevice,
                                     const VkExtent2D &size,
                                     uint32_t mip_level,
                                     VkFormat format,
                                     VkImageUsageFlags usage,
                                     std::vector<uint32_t> &accessingQueues,
                                     VkAccessFlags initialAccessMask,
                                     VkPipelineStageFlags initialPipelineStage) {
    return Image::create(pDevice, size, mip_level, format, VMA_MEMORY_USAGE_GPU_ONLY, usage, accessingQueues,
                         initialAccessMask,
                         initialPipelineStage);
}

std::shared_ptr<Image> Image::create(const std::shared_ptr<Device>& pDevice,
                                     const VkExtent2D &size,
                                     uint32_t mip_level,
                                     VkFormat format,
                                     VmaMemoryUsage memoryUsage,
                                     VkImageUsageFlags usage,
                                     std::vector<uint32_t> &accessingQueues,
                                     VkAccessFlags initialAccessMask,
                                     VkPipelineStageFlags initialPipelineStage) {
    auto image = std::make_shared<Image>();
    image->device = pDevice;
    image->currentAccessFlags = initialAccessMask;
    image->currentStage = initialPipelineStage;

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
    allocationCreateInfo.usage = memoryUsage;
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

std::shared_ptr<Image> Image::createHostStagingImage(const std::shared_ptr<Device>& pDevice,
                                                      const VkExtent2D& size,
                                                      VkFormat format,
                                                      std::vector<uint32_t> &accessingQueues) {

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent = { size.width, size.height, 0 };
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

    return Image::create(pDevice, VMA_MEMORY_USAGE_UNKNOWN, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                          createInfo, accessingQueues);
}

void Image::transferDataMapped(void *src) {
    void *dst;
    vmaMapMemory(device->getAllocator(), allocation, &dst);
    memcpy(dst, src, allocationInfo.size);
    vmaUnmapMemory(device->getAllocator(), allocation);
}

void Image::transferDataMapped(void *src, size_t size) {
    void *dst;
    vmaMapMemory(device->getAllocator(), allocation, &dst);
    memcpy(dst, src, size);
    vmaUnmapMemory(device->getAllocator(), allocation);
}

void Image::transferDataStaged(void *src, const std::shared_ptr<CommandPool>& commandPool) {
    transferDataStaged(src, commandPool, allocationInfo.size);
}

void Image::transferDataStaged(void *src, const std::shared_ptr<CommandPool>& commandPool, VkDeviceSize size,
                                VkDeviceSize offset) {
    std::vector<uint32_t> accessingQueues = { commandPool->getQueueFamily()->getQueueFamilyIndex() };
    auto stagingImage = Image::createHostStagingImage(device, { extent.width, extent.height }, format, accessingQueues);

    VkImageCopy imageCopy{};
    imageCopy.extent = extent;
    imageCopy.dstOffset = {0, 0, 0};
    imageCopy.srcOffset = {0, 0, 0};
    imageCopy.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1};
    imageCopy.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1};

    auto commandBuffer = CommandBuffer::create(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    stagingImage->transitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_HOST_WRITE_BIT);
    stagingImage->transferDataMapped(src, size);
    stagingImage->transitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT);
    transitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);
    commandBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyImage(stagingImage, shared_from_this(), { imageCopy }, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                             currentLayout);
    transitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);
    commandBuffer->endCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer->getHandlePtr();

    commandPool->getQueue()->submitCommandBuffer({ submitInfo });
    commandPool->getQueue()->waitForIdle();
}

std::shared_ptr<ImageView> Image::createImageView(VkImageViewType viewType, VkImageSubresourceRange subresourceRange) {
    return ImageView::create(shared_from_this(), viewType, subresourceRange);
}

void Image::transitionImageLayout(const std::shared_ptr<CommandPool> &commandPool,
                                  VkImageLayout dst) {
    std::vector<uint32_t> accessingQueues = { commandPool->getQueueFamily()->getQueueFamilyIndex() };

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = currentLayout;
    barrier.newLayout = dst;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = handle;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    auto commandBuffer = CommandBuffer::create(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    commandBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkCmdPipelineBarrier(commandBuffer->getHandle(),
                         0, 0,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);
    commandBuffer->endCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer->getHandlePtr();

    commandPool->getQueue()->submitCommandBuffer({ submitInfo });
    commandPool->getQueue()->waitForIdle();
}

void Image::transitionImageLayout(const std::shared_ptr<CommandBuffer> &commandBuffer,
                                  VkImageLayout dst,
                                  VkPipelineStageFlags dstStage,
                                  VkAccessFlags dstMask) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = currentLayout;
    barrier.newLayout = dst;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = handle;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = currentAccessFlags;
    barrier.dstAccessMask = dstMask;

    currentAccessFlags = dstMask;

    vkCmdPipelineBarrier(commandBuffer->getHandle(),
                         currentStage, dstStage,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    currentStage = dstStage;
}

void *Image::map() {
    if (allocationInfo.pMappedData != nullptr)
        return allocationInfo.pMappedData;

    void *data;
    vmaMapMemory(device->getAllocator(), allocation, &data);
    return data;
}

void Image::unmap() {
    if (allocationInfo.pMappedData == nullptr)
        vmaUnmapMemory(device->getAllocator(), allocation);
}

Image::~Image() {
    vmaDestroyImage(device->getAllocator(), handle, allocation);
}
