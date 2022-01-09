//
// Created by nic on 03/07/2021.
//

#pragma once

#ifndef CUBICAD_SRC_RENDERER_VULKAN_COMMANDBUFFER_H_
#define CUBICAD_SRC_RENDERER_VULKAN_COMMANDBUFFER_H_

#include <vulkan/vulkan_core.h>

#include "vulkanclass.h"
#include "commandpool.h"
#include "semaphore.h"
#include "renderpass.h"
#include "framebuffer.h"
#include "graphicspipeline.h"
#include "pipelinebase.h"
#include "pipelinelayout.h"
#include "descriptorset.h"


class GraphicsPipeline;

class FrameBuffer;

class CommandBuffer : public VulkanClass<VkCommandBuffer> {
  private:
    std::shared_ptr<CommandPool> commandPool;

  public:
    static std::shared_ptr<CommandBuffer> create(const std::shared_ptr<CommandPool> &pCommandPool,
                                                 VkCommandBufferLevel level);

    void submitToQueue(std::vector<std::shared_ptr<Semaphore>> &signalSemaphores,
                       std::vector<VkPipelineStageFlags> waitStageMask,
                       std::vector<std::shared_ptr<Semaphore>> &waitSemaphores,
                       const std::shared_ptr<Queue> &queue,
                       const std::shared_ptr<Fence>& triggerFence);

    void beginCommandBuffer(VkCommandBufferUsageFlags flags = 0);
    void resetCommandBuffer(VkCommandBufferResetFlags flags = 0);
    void endCommandBuffer();

    void beginRenderPass(const std::shared_ptr<RenderPass> &renderPass,
                         const std::shared_ptr<FrameBuffer> &frameBuffer,
                         std::vector<VkClearValue> &clearValues,
                         uint32_t frameIndex,
                         VkExtent2D extend,
                         VkOffset2D offset,
                         bool secondary = false);
    void nextSubpass(VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE);
    void endRenderPass();

    void bindPipeline(const std::shared_ptr<GraphicsPipeline> &pipeline);
    void bindDescriptorSets(std::vector<std::shared_ptr<DescriptorSet>> &descriptorSets,
                            const std::shared_ptr<PipelineLayout> &layout,
                            VkPipelineBindPoint bindPoint,
                            std::vector<uint32_t> &offsets);
    void bindDescriptorSets(std::vector<std::shared_ptr<DescriptorSet>> &descriptorSets,
                            const std::shared_ptr<PipelineBase> &pipeline,
                            std::vector<uint32_t> &offsets);
    void bindDescriptorSets(std::vector<std::shared_ptr<DescriptorSet>> &descriptorSets,
        const std::shared_ptr<PipelineBase> &pipeline);
    void bindVertexBuffer(const std::shared_ptr<Buffer> &buffer, uint32_t slot, VkDeviceSize offset = 0);
    void bindIndexBuffer(const std::shared_ptr<Buffer> &buffer, VkIndexType type, VkDeviceSize offset = 0);

    void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
    void drawIndexed(uint32_t indexCount,
                     uint32_t instanceCount = 0,
                     uint32_t firstIndex = 0,
                     int32_t vertexOffset = 0,
                     uint32_t firstInstance = 0);

    void copyBuffer(const std::shared_ptr<Buffer>& src, const std::shared_ptr<Buffer>& dst, std::vector<VkBufferCopy>
        copyRegions);
};

#endif //CUBICAD_SRC_RENDERER_VULKAN_COMMANDBUFFER_H_
