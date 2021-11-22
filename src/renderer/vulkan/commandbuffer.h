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


class   CommandBuffer : public VulkanClass<VkCommandBuffer> {
  private:
    CommandPool *commandPool;

  public:
    CommandBuffer(CommandPool *pCommandPool, VkCommandBufferLevel level);

    void submitToQueue(std::vector<Semaphore*>& signalSemaphores,
                               std::vector<VkPipelineStageFlags> waitStageMask,
                               std::vector<Semaphore*>& waitSemaphores,
                               Queue* queue,
                               Fence* triggerFence);

    void beginCommandBuffer(VkCommandBufferUsageFlags flags = 0);
    void resetCommandBuffer(VkCommandBufferResetFlags flags = 0);
    void endCommandBuffer();

    void beginRenderPass(RenderPass *renderPass,
                         FrameBuffer *frameBuffer,
                         std::vector<VkClearValue> &clearValues,
                         uint32_t frameIndex,
                         VkExtent2D extend,
                         VkOffset2D offset,
                         bool secondary = false);
    void nextSubpass(VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE);
    void endRenderPass();

    void bindPipeline(GraphicsPipeline *pipeline);
    void bindDescriptorSets(std::vector<DescriptorSet *> &descriptorSets,
                            PipelineLayout *layout,
                            VkPipelineBindPoint bindPoint,
                            std::vector<uint32_t>& offsets);
    void bindDescriptorSets(std::vector<DescriptorSet *> &descriptorSets,
                            PipelineBase *pipeline,
                            std::vector<uint32_t>& offsets);
    void bindVertexBuffer(Buffer *buffer, uint32_t slot, VkDeviceSize offset = 0);
    void bindIndexBuffer(Buffer *buffer, VkIndexType type, VkDeviceSize offset = 0);

    void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
    void drawIndexed(uint32_t indexCount,
                     uint32_t instanceCount = 0,
                     uint32_t firstIndex = 0,
                     int32_t vertexOffset = 0,
                     uint32_t firstInstance = 0);

};

#endif //CUBICAD_SRC_RENDERER_VULKAN_COMMANDBUFFER_H_
