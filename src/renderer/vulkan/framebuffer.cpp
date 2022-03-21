//
// Created by nic on 03/08/2021.
//

#include "framebuffer.h"

#include <utility>


std::shared_ptr<FrameBuffer> FrameBuffer::create(const std::shared_ptr<Device> &pDevice,
                                                 const std::shared_ptr<RenderPass> &renderPass,
                                                 VkExtent2D extent,
                                                 std::vector<std::vector<std::shared_ptr<ImageView>>> &imageViews) {
    auto frameBuffer = std::make_shared<FrameBuffer>();
    frameBuffer->device = pDevice;
    frameBuffer->frameBuffers.resize(imageViews.size());
    frameBuffer->extent = extent;

    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass->getHandle();
    createInfo.width = extent.width;
    createInfo.height = extent.height;
    createInfo.layers = 1;

    for (uint32_t i = 0; i < imageViews.size(); i++) {
        std::vector<VkImageView> vulkanImageViews{};
        for (auto &imageView: imageViews[i])
            vulkanImageViews.push_back(imageView->getHandle());

        createInfo.attachmentCount = static_cast<uint32_t>(vulkanImageViews.size());
        createInfo.pAttachments = vulkanImageViews.data();

        if (vkCreateFramebuffer(frameBuffer->device->getHandle(), &createInfo, nullptr, &frameBuffer->frameBuffers[i])
            != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    return frameBuffer;
}

std::shared_ptr<FrameBuffer> FrameBuffer::create(std::shared_ptr<Device> pDevice,
                                                 const std::shared_ptr<RenderPass> &renderPass,
                                                 const std::shared_ptr<SwapChain> &swapChain,
                                                 std::vector<std::vector<std::shared_ptr<ImageView>>> &imageViews) {
    auto frameBuffer = std::make_shared<FrameBuffer>();
    frameBuffer->device = std::move(pDevice);
    frameBuffer->frameBuffers.resize(swapChain->getImageCount());
    frameBuffer->extent = swapChain->getSwapExtent();

    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass->getHandle();
    createInfo.width = frameBuffer->extent.width;
    createInfo.height = frameBuffer->extent.height;
    createInfo.layers = 1;

    for (uint32_t i = 0; i < swapChain->getImageCount(); i++) {
        std::vector<VkImageView> vulkanImageViews{};
        vulkanImageViews.push_back(swapChain->getSwapChainImageView(i));
        for (auto &imageView: imageViews[i])
            vulkanImageViews.push_back(imageView->getHandle());

        createInfo.attachmentCount = static_cast<uint32_t>(vulkanImageViews.size());
        createInfo.pAttachments = vulkanImageViews.data();

        if (vkCreateFramebuffer(frameBuffer->device->getHandle(), &createInfo, nullptr, &frameBuffer->frameBuffers[i])
            != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    return frameBuffer;
}

FrameBuffer::~FrameBuffer() {
    for (auto &buffer: frameBuffers)
        vkDestroyFramebuffer(device->getHandle(), buffer, nullptr);
}
