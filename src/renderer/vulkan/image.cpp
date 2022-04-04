//
// Created by nic on 16/07/2021.
//

#include <vulkan/vulkan_core.h>

#include <utility>

#include "image.h"


std::shared_ptr<Image> Image::create(const std::shared_ptr<Device> &pDevice,
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

std::shared_ptr<Image> Image::create(const std::shared_ptr<Device> &pDevice,
                                     const VkExtent2D &size,
                                     uint32_t mip_level,
                                     VkFormat format,
                                     VkImageUsageFlags usage,
                                     std::vector<uint32_t> &accessingQueues,
                                     VkAccessFlags initialAccessMask,
                                     VkPipelineStageFlags initialPipelineStage) {
    return Image::create(pDevice,
                         size,
                         mip_level,
                         format,
                         VMA_MEMORY_USAGE_GPU_ONLY,
                         usage,
                         accessingQueues,
                         initialAccessMask,
                         initialPipelineStage);
}

std::shared_ptr<Image> Image::create(const std::shared_ptr<Device> &pDevice,
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

std::shared_ptr<Image> Image::createHostStagingImage(const std::shared_ptr<Device> &pDevice,
                                                     const VkExtent2D &size,
                                                     VkFormat format,
                                                     std::vector<uint32_t> &accessingQueues) {

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent = {size.width, size.height, 0};
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
    createInfo.format = format;

    return Image::create(pDevice,
                         VMA_MEMORY_USAGE_UNKNOWN,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         createInfo,
                         accessingQueues);
}

std::shared_ptr<Buffer> Image::createHostStagingBuffer(const std::shared_ptr<Device> &pDevice,
                                                       VkDeviceSize size,
                                                       std::vector<uint32_t> &accessingQueues) {
    return Buffer::create(pDevice,
                          size,
                          VMA_MEMORY_USAGE_CPU_ONLY,
                          0,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          accessingQueues);
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

void Image::transferDataStaged(void *src, const std::shared_ptr<CommandPool> &commandPool) {
    transferDataStaged(src, commandPool, allocationInfo.size);
}

void Image::transferDataStaged(void *src,
                               const std::shared_ptr<CommandPool> &commandPool,
                               VkDeviceSize size, [[maybe_unused]] VkDeviceSize offset,
                               VkPipelineStageFlags dstStage) {
    std::vector<uint32_t> accessingQueues = {commandPool->getQueueFamily()->getQueueFamilyIndex()};
    auto stagingBuffer = Image::createHostStagingBuffer(device, allocationInfo.size, accessingQueues);

    VkBufferImageCopy imageCopy{};
    imageCopy.imageExtent = {extent.width, extent.height, 1};
    imageCopy.imageOffset = {0, 0, 0};

    imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopy.imageSubresource.mipLevel = 0;
    imageCopy.imageSubresource.baseArrayLayer = 0;
    imageCopy.imageSubresource.layerCount = 1;

    imageCopy.bufferOffset = 0;
    imageCopy.bufferRowLength = 0;
    imageCopy.bufferImageHeight = 0;

    auto commandBuffer = CommandBuffer::create(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    commandBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    stagingBuffer->transferDataMapped(src, size);
    transitionImageLayout(commandBuffer,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_PIPELINE_STAGE_TRANSFER_BIT,
                          VK_ACCESS_TRANSFER_WRITE_BIT);
    commandBuffer->copyBufferImage(stagingBuffer, shared_from_this(), {imageCopy}, currentLayout);
    commandBuffer->endCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer->getHandlePtr();

    commandPool->getQueue()->submitCommandBuffer({submitInfo});
    commandPool->getQueue()->waitForIdle();
}

std::shared_ptr<ImageView> Image::createImageView(VkImageViewType viewType, VkImageSubresourceRange subresourceRange) {
    return ImageView::create(shared_from_this(), viewType, subresourceRange);
}

void Image::transitionImageLayout(const std::shared_ptr<CommandPool> &commandPool, VkImageLayout dst) {
    std::vector<uint32_t> accessingQueues = {commandPool->getQueueFamily()->getQueueFamilyIndex()};

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
    vkCmdPipelineBarrier(commandBuffer->getHandle(), 0, 0, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    commandBuffer->endCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer->getHandlePtr();

    commandPool->getQueue()->submitCommandBuffer({submitInfo});
    commandPool->getQueue()->waitForIdle();
}

void Image::transitionImageLayout(const std::shared_ptr<CommandBuffer> &commandBuffer,
                                  VkImageLayout dst,
                                  VkPipelineStageFlags dstStage,
                                  VkAccessFlags dstMask,
                                  VkImageAspectFlags imageAspect) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = currentLayout;
    barrier.newLayout = dst;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = handle;
    barrier.subresourceRange.aspectMask = imageAspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = currentAccessFlags;
    barrier.dstAccessMask = dstMask;

    currentAccessFlags = dstMask;

    vkCmdPipelineBarrier(commandBuffer->getHandle(), currentStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    currentStage = dstStage;
    currentLayout = dst;
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

void Image::generateMipmaps(const std::shared_ptr<CommandPool> &commandPool) {
    auto commandBuffer = CommandBuffer::create(commandPool);
    commandBuffer->beginCommandBuffer();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = handle;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    auto mipWidth = static_cast<int32_t>(extent.width);
    auto mipHeight = static_cast<int32_t>(extent.height);

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer->getHandle(),
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer->getHandle(),
                       handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer->getHandle(),
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer->getHandle(),
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    transitionImageLayout(commandBuffer,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                              VK_ACCESS_SHADER_READ_BIT);

    commandBuffer->endCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer->getHandlePtr();

    vkQueueSubmit(commandPool->getQueue()->getHandle(), 1, &submitInfo, VK_NULL_HANDLE);
}
