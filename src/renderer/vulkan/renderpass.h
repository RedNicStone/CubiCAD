//
// Created by nic on 19/05/2021.
//


#pragma once

#ifndef CUBICAD_RENDERPASS_H
#define CUBICAD_RENDERPASS_H

#include <vulkan/vulkan.h>

#include <vector>
#include <array>

#include "device.h"
#include "vulkanclass.h"
#include "swapchain.h"


// todo resolve attachments via dependency graph

// todo pass image info when submitting to render

enum RenderPassAttachmentType : uint8_t {
    RENDER_PASS_ATTACHMENT_TYPE_INVALID = 127,
    RENDER_PASS_ATTACHMENT_TYPE_UNKNOWN = 0,
    RENDER_PASS_ATTACHMENT_TYPE_INPUT = 1,
    RENDER_PASS_ATTACHMENT_TYPE_COLOR = 2,
    RENDER_PASS_ATTACHMENT_TYPE_RESOLVE = 3,
    RENDER_PASS_ATTACHMENT_TYPE_DEPTH_STENCIL = 4,
    RENDER_PASS_ATTACHMENT_TYPE_PRESERVE = 5,  // todo preserve attachments are indices
    RENDER_PASS_ATTACHMENT_TYPES_COUNT = 5
};

class SwapChain;

//todo implement multiple subpasses, dependencies and color / input attachments
class RenderPass : public VulkanClass<VkRenderPass> {
  private:
    std::shared_ptr<Device> device;

    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDescription> subpasses;
    std::vector<VkPipelineStageFlags> subpass_stages;
    std::vector<VkSubpassDependency> dependencies;

  public:
    static std::shared_ptr<RenderPass> create(std::shared_ptr<Device> pDevice);

    std::shared_ptr<Device> getDevice() { return device; }

    uint32_t submitImageAttachment(VkFormat format,
                                   VkSampleCountFlagBits sampleCount,
                                   VkAttachmentLoadOp loadOp,
                                   VkAttachmentStoreOp storeOp,
                                   VkAttachmentLoadOp stencilLoadOp,
                                   VkAttachmentStoreOp stencilStoreOp,
                                   VkImageLayout initialLayout,
                                   VkImageLayout finalLayout);

    uint32_t submitSwapChainAttachment(const std::shared_ptr<SwapChain> &swapChain, bool clearImage);

    uint32_t submitSubpass(VkPipelineBindPoint bindPoint,
                           std::vector<VkAttachmentReference> &colorAttachments,
                           std::vector<VkAttachmentReference> &inputAttachments,
                           VkAttachmentReference *depthAttachment,
                           VkPipelineStageFlags stage);

    void submitDependency(uint32_t src, uint32_t dst, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                          VkPipelineStageFlags srcPipeline = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VkPipelineStageFlags dstPipeline = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    void build();

    ~RenderPass();
};

#endif //CUBICAD_RENDERPASS_H
