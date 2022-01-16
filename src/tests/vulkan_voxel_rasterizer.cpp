//
// Created by nic on 14/03/2021.
//

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <vector>
#include <glm/glm.hpp>

#include "../renderer/vulkan/window.h"
#include "../renderer/vulkan/swapchain.h"
#include "../renderer/vulkan/renderpass.h"
#include "../renderer/vulkan/image.h"
#include "../renderer/vulkan/graphicspipeline.h"
#include "../renderer/vulkan/framebuffer.h"
#include "../renderer/vulkan/commandpool.h"
#include "../renderer/vulkan/commandbuffer.h"
#include "../renderer/vulkan/uniformbuffer.h"

#include <vulkan/vulkan.h>


int main() {
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    uint32_t version = VK_MAKE_VERSION(0, 3, 0);

    glfwInit();

    auto instance = Instance::create("api_test", version, true);

    Window::initWindows();
    auto window = Window::create(instance, "API Test " + std::to_string(version), 1080, 720);

    auto physical_device = instance->getPhysicalDevice()[0];
    physical_device->queryDeviceProperties();

    VkPhysicalDeviceFeatures features{};
    std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    auto familyHandler = physical_device->getQueueFamilyHandler();

    optional<uint32_t> graphicsFamily = familyHandler->getOptimalQueueFamily(VK_QUEUE_GRAPHICS_BIT);
    if (graphicsFamily.has_value())
        familyHandler->registerQueueFamilyAllocation(graphicsFamily.value(), 1);

    optional<uint32_t> transferFamily = familyHandler->getOptimalQueueFamily(VK_QUEUE_TRANSFER_BIT);
    if (transferFamily.has_value())
        familyHandler->registerQueueFamilyAllocation(transferFamily.value(), 1);

    optional<uint32_t> presentFamily = familyHandler->getOptimalPresentQueue(window);
    if (presentFamily.has_value())
        familyHandler->registerQueueFamilyAllocation(presentFamily.value(), 1);

    std::vector<uint32_t> swapChainQueues = familyHandler->getRegisteredQueueFamilies();

    auto device = Device::create(physical_device, deviceExtensions, features);

    auto presentQueue = physical_device->getQueueFamilyHandler()->getQueueFamily(presentFamily.value())->getQueue(0);
    auto graphicsQueue = physical_device->getQueueFamilyHandler()->getQueueFamily(graphicsFamily.value())->getQueue(0);
    auto transferQueue = physical_device->getQueueFamilyHandler()->getQueueFamily(transferFamily.value())->getQueue(0);

    std::cout << "Graphics queue: " << graphicsQueue->getHandle() << std::endl;
    std::cout << "Present queue: " << presentQueue->getHandle() << std::endl;

    auto swapChain = SwapChain::create(device, window, presentQueue, 3, swapChainQueues);

    auto renderPass = RenderPass::create(device);
    std::vector<VkAttachmentReference>
        attachments =
        {{renderPass->submitSwapChainAttachment(swapChain, true), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};
    uint32_t
        renderPassID =
        renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                 attachments,
                                 nullptr,
                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    renderPass->submitDependency(VK_SUBPASS_EXTERNAL, renderPassID, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    renderPass->build();

    struct FrameInfoObject {
        glm::mediump_uint32_t frameCount;
        glm::mediump_uint32_t frameID;
        glm::mediump_uint32_t frameTime;
        glm::mediump_uint32_t time;
    };

    std::vector<VkDescriptorPoolSize> poolSizes(1);
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = swapChain->getImageCount();

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(1);
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorCount = 1;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindings[0].pImmutableSamplers = nullptr;
    setLayoutBindings[0].stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorLayouts{};
    descriptorLayouts.push_back(DescriptorSetLayout::create(device, setLayoutBindings));

    auto layout = PipelineLayout::create(device, descriptorLayouts);

    auto descriptorPool = DescriptorPool::create(device, poolSizes);

    std::vector<std::shared_ptr<UniformBuffer<FrameInfoObject>>> frameInfoBuffers{};
    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};

    frameInfoBuffers.reserve(swapChain->getImageCount());
    descriptorSets.reserve(swapChain->getImageCount());

    for (uint32_t i = 0; i < swapChain->getImageCount(); i++) {
        frameInfoBuffers.push_back(UniformBuffer<FrameInfoObject>::create(device, transferQueue));
        descriptorSets.push_back(DescriptorSet::create(descriptorLayouts[0], descriptorPool));
        descriptorSets[i]->updateUniformBuffer(frameInfoBuffers[i], 0);
    }

    auto vertexShader = VertexShader::create(device, "main", "resources/shaders/compiled/quad.vert.spv");
    auto fragmentShader = FragmentShader::create(device, "main", "resources/shaders/compiled/uv.frag.spv");
    std::vector<std::shared_ptr<GraphicsShader>> shaders = {vertexShader, fragmentShader};

    auto pipeline = GraphicsPipeline::create(device, layout, shaders, renderPass, swapChain->getSwapExtent());

    std::vector<std::vector<std::shared_ptr<ImageView>>> imageViews(swapChain->getImageCount(),
                                                                    std::vector<std::shared_ptr<ImageView>>
        ());

    auto frameBuffer = FrameBuffer::create(device, renderPass, swapChain, imageViews);

    auto graphicsPool = CommandPool::create(device, graphicsQueue, 0);
    auto presentPool = CommandPool::create(device, presentQueue, 0);

    std::vector<std::shared_ptr<CommandBuffer>> graphicsBuffers{};

    graphicsBuffers.reserve(swapChain->getImageCount());
    for (uint32_t i = 0; i < swapChain->getImageCount(); i++) {
        graphicsBuffers.push_back(CommandBuffer::create(graphicsPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY));

        graphicsBuffers[i]->beginCommandBuffer();

        std::vector<VkClearValue> clearColors = {{{{0.0f, 0.0f, 0.0f, 1.0f}}}};
        graphicsBuffers[i]->beginRenderPass(renderPass, frameBuffer, clearColors, i, swapChain->getSwapExtent(),
                                           VkOffset2D{0, 0});

        graphicsBuffers[i]->bindPipeline(pipeline);

        std::vector<std::shared_ptr<DescriptorSet>> pDescriptorSets = { descriptorSets[i] };
        std::vector<uint32_t> offsets { };
        graphicsBuffers[i]->bindDescriptorSets(pDescriptorSets, pipeline, offsets);

        graphicsBuffers[i]->draw(6);

        graphicsBuffers[i]->endRenderPass();

        graphicsBuffers[i]->endCommandBuffer();
    }

    dexode::EventBus::Listener listener{publicRenderBus.getBus()};
    listener.listen<RenderSubmitQueueEvent>([&](const auto &event) {
        //swapChain.getPresentFence()->waitForSignal();
        //swapChain.getPresentFence()->resetState();

        std::vector<std::shared_ptr<Semaphore>> signalSemaphores = swapChain->getRenderSignalSemaphores();
        std::vector<std::shared_ptr<Semaphore>> waitSemaphores = swapChain->getRenderWaitSemaphores();
        std::cout << "submitting using image index: " << swapChain->getCurrentImageIndex() << std::endl;
        std::cout << "submitting using image: " << swapChain->getCurrentFrame() << std::endl;
        graphicsBuffers[swapChain->getCurrentImageIndex()]->submitToQueue(signalSemaphores,
                                                                   { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
                                                                   waitSemaphores,
                                                                   graphicsQueue,
                                                                   swapChain->getPresentFence());


        while (graphicsQueue->hasWorkSubmitted()) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
    });

    static auto startTime = std::chrono::high_resolution_clock::now();
    auto frameTime = startTime;
    listener.listen<PreRenderUpdateBuffersEvent>([&](const auto &event) {
        FrameInfoObject* handle = frameInfoBuffers[swapChain->getCurrentImageIndex()]->getDataHandle();
        handle->frameCount++;
        handle->frameID = static_cast<unsigned char>(swapChain->getCurrentImageIndex());

        auto currentTime = std::chrono::high_resolution_clock::now();

        handle->frameTime = std::chrono::duration<uint16_t, std::chrono::nanoseconds::period>
            (currentTime - frameTime).count();
        handle->time = std::chrono::duration<uint32_t, std::chrono::nanoseconds::period>
            (currentTime - startTime).count();

        frameTime = startTime;
    });

    publicRenderBus.getBus()->process();

    /*while (true) {
        //swapChain->doRenderLoop(&graphicsBuffers, graphicsQueue);
        //publicRenderBus.runRenderLoop(device);
    }*/

    auto imageAvailableSemaphore = Semaphore::create(device);
    auto renderFinishedSemaphore = Semaphore::create(device);

    while (true) {
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device->getHandle(), swapChain->getHandle(), UINT64_MAX, imageAvailableSemaphore->getHandle(),
                              VK_NULL_HANDLE, &imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore->getHandle()};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        VkCommandBuffer buffer = graphicsBuffers[imageIndex]->getHandle();
        submitInfo.pCommandBuffers = &buffer;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore->getHandle()};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue->getHandle(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain->getHandle()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional

        vkQueuePresentKHR(presentQueue->getHandle(), &presentInfo);
    }
}