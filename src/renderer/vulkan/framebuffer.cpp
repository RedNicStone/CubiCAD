//
// Created by nic on 03/08/2021.
//

#include "framebuffer.h"


FrameBuffer::FrameBuffer(Device *device,
                         RenderPass *renderPass,
                         VkExtent2D extent,
                         std::vector<std::vector<ImageView>> &imageViews) : device(device) {
    framebuffers.resize(imageViews.size());

    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass->getHandle();
    createInfo.width = extent.width;
    createInfo.height = extent.height;
    createInfo.layers = 1;

    for (uint32_t i = 0; i > imageViews.size(); i++) {
        std::vector<VkImageView> vulkanImageViews(imageViews.size());
        for (auto &imageView : imageViews[i])
            vulkanImageViews.push_back(imageView.getHandle());

        createInfo.attachmentCount = static_cast<uint32_t>(vulkanImageViews.size());
        createInfo.pAttachments = vulkanImageViews.data();

        if (vkCreateFramebuffer(device->getHandle(), &createInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

FrameBuffer::FrameBuffer(Device *device,
                         RenderPass *renderPass,
                         SwapChain *swapChain,
                         std::vector<std::vector<ImageView>> &imageViews) : device(device) {
    framebuffers.resize(imageViews.size());

    VkExtent2D extend = swapChain->getSwapExtent();

    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass->getHandle();
    createInfo.width = extend.width;
    createInfo.height = extend.height;
    createInfo.layers = 1;

    for (uint32_t i = 0; i < swapChain->getImageCount(); i++) {
        std::vector<VkImageView> vulkanImageViews{};
        vulkanImageViews.push_back(swapChain->getSwapChainImageView(i));
        for (auto &imageView : imageViews[i])
            vulkanImageViews.push_back(imageView.getHandle());

        createInfo.attachmentCount = static_cast<uint32_t>(vulkanImageViews.size());
        createInfo.pAttachments = vulkanImageViews.data();

        if (vkCreateFramebuffer(device->getHandle(), &createInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

FrameBuffer::~FrameBuffer() {
    for (auto &buffer : framebuffers)
        vkDestroyFramebuffer(device->getHandle(), buffer, nullptr);
}
