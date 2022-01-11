//
// Created by nic on 03/07/2021.
//

#include "commandbuffer.h"

#include <utility>


std::shared_ptr<CommandBuffer> CommandBuffer::create(const std::shared_ptr<CommandPool> &pCommandPool,
                                                     VkCommandBufferLevel level) {
    auto commandBuffer = std::make_shared<CommandBuffer>();

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = level;
    allocateInfo.commandPool = pCommandPool->getHandle();
    allocateInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(pCommandPool->getDevice()->getHandle(), &allocateInfo, &commandBuffer->handle);

    return commandBuffer;
}

void CommandBuffer::submitToQueue(std::vector<std::shared_ptr<Semaphore>> &signalSemaphores,
                                  std::vector<VkPipelineStageFlags> waitStageMask,
                                  std::vector<std::shared_ptr<Semaphore>> &waitSemaphores,
                                  const std::shared_ptr<Queue> &queue,
                                  const std::shared_ptr<Fence>& triggerFence) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &handle;

    std::vector<VkSemaphore> signalSemaphoreHandles;
    signalSemaphoreHandles.reserve(signalSemaphores.size());
    for (auto &signalSemaphore: signalSemaphores)
        signalSemaphoreHandles.push_back(signalSemaphore->getHandle());
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphoreHandles.size());
    submitInfo.pSignalSemaphores = signalSemaphoreHandles.data();

    std::vector<VkSemaphore> waitSemaphoreHandles;
    waitSemaphoreHandles.reserve(waitSemaphores.size());
    for (auto &waitSemaphore: waitSemaphores)
        waitSemaphoreHandles.push_back(waitSemaphore->getHandle());
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphoreHandles.size());
    submitInfo.pWaitSemaphores = waitSemaphoreHandles.data();
    submitInfo.pWaitDstStageMask = waitStageMask.data();

    queue->submitCommandBuffer({submitInfo}, triggerFence);
}

void CommandBuffer::beginCommandBuffer(VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = nullptr;  //todo add inheritance

    if (vkBeginCommandBuffer(handle, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to being recording command buffer");
}

void CommandBuffer::resetCommandBuffer(VkCommandBufferResetFlags flags) {
    if (vkResetCommandBuffer(handle, flags) != VK_SUCCESS)
        throw std::runtime_error("failed to reset command buffer");
}

void CommandBuffer::endCommandBuffer() {
    if (vkEndCommandBuffer(handle) != VK_SUCCESS)
        throw std::runtime_error("failed to record command buffer");
}

void CommandBuffer::beginRenderPass(const std::shared_ptr<RenderPass> &renderPass,
                                    const std::shared_ptr<FrameBuffer> &frameBuffer,
                                    std::vector<VkClearValue> &clearValues,
                                    uint32_t frameIndex,
                                    VkExtent2D extend,
                                    VkOffset2D offset,
                                    bool secondary) {
    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = renderPass->getHandle();
    beginInfo.framebuffer = frameBuffer->getFramebuffer(frameIndex);
    beginInfo.renderArea.extent = extend;
    beginInfo.renderArea.offset = offset;
    beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    beginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(handle,
                         &beginInfo,
                         secondary ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::nextSubpass(VkSubpassContents subpassContents) {
    vkCmdNextSubpass(handle, subpassContents);
}

void CommandBuffer::endRenderPass() {
    vkCmdEndRenderPass(handle);
}

void CommandBuffer::bindPipeline(const std::shared_ptr<GraphicsPipeline> &pipeline) {
    vkCmdBindPipeline(handle, pipeline->getBindPoint(), pipeline->getHandle());
}

void CommandBuffer::bindDescriptorSets(std::vector<std::shared_ptr<DescriptorSet>> &descriptorSets,
                                       const std::shared_ptr<PipelineLayout> &layout,
                                       VkPipelineBindPoint bindPoint,
                                       std::vector<uint32_t> &offsets) {
    std::vector<VkDescriptorSet> descriptorHandles(descriptorSets.size());
    for (size_t i = 0; i < descriptorSets.size(); i++) {
        descriptorHandles[i] = descriptorSets[i]->getHandle();
    }
    vkCmdBindDescriptorSets(handle,
                            bindPoint,
                            layout->getHandle(),
                            0,
                            static_cast<uint32_t>(descriptorHandles.size()),
                            descriptorHandles.data(),
                            static_cast<uint32_t>(offsets.size()),
                            offsets.data());
}

void CommandBuffer::bindDescriptorSets(std::vector<std::shared_ptr<DescriptorSet>> &descriptorSets,
                                       const std::shared_ptr<PipelineBase> &pipeline,
                                       std::vector<uint32_t> &offsets) {
    bindDescriptorSets(descriptorSets, pipeline->getLayout(), pipeline->getBindPoint(), offsets);
}

void CommandBuffer::bindDescriptorSets(std::vector<std::shared_ptr<DescriptorSet>> &descriptorSets,
                                                                       const std::shared_ptr<PipelineBase> &pipeline) {
    std::vector<uint32_t> offsets{};
    bindDescriptorSets(descriptorSets, pipeline->getLayout(), pipeline->getBindPoint(), offsets);
}

void CommandBuffer::bindVertexBuffer(const std::shared_ptr<Buffer> &buffer, uint32_t slot, VkDeviceSize offset) {
    VkBuffer bufferHandle = buffer->getHandle();
    vkCmdBindVertexBuffers(handle, slot, 1, &bufferHandle, &offset);
}

void CommandBuffer::bindIndexBuffer(const std::shared_ptr<Buffer> &buffer, VkIndexType type, VkDeviceSize offset) {
    vkCmdBindIndexBuffer(handle, buffer->getHandle(), offset, type);
}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    vkCmdDraw(handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint32_t indexCount,
                                uint32_t instanceCount,
                                uint32_t firstIndex,
                                int32_t vertexOffset,
                                uint32_t firstInstance) {
    vkCmdDrawIndexed(handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::drawIndirect(const std::shared_ptr<Buffer> &buffer, uint32_t
drawCount, VkDeviceSize offset, uint32_t stride) {
    vkCmdDrawIndirect(handle, buffer->getHandle(), offset, drawCount, stride);
}

void CommandBuffer::drawIndexedIndirect(const std::shared_ptr<Buffer> &buffer, uint32_t
drawCount, VkDeviceSize offset, uint32_t stride) {
    vkCmdDrawIndexedIndirect(handle, buffer->getHandle(), offset, drawCount, stride);
}

void CommandBuffer::copyBuffer(const std::shared_ptr<Buffer>& src,
                               const std::shared_ptr<Buffer>& dst,
                               std::vector<VkBufferCopy> copyRegions) {
    vkCmdCopyBuffer(handle, src->getHandle(), dst->getHandle(), static_cast<uint32_t>(copyRegions.size()), copyRegions.data());
}
