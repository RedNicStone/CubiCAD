//
// Created by nic on 07/12/2021.
//

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>


#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <bitset>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>

#include <glm/glm.hpp>
#include <unordered_set>

#include "../renderer/vulkan/window.h"
#include "../renderer/vulkan/swapchain.h"
#include "../renderer/mesh.h"


using ComplexNum = std::pair<long double, long double>;

const uint32_t WIDTH = 1080;
const uint32_t HEIGHT = 720;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

const uint32_t debugLevel = 3;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct MandelbrotUBO {
    uint frameCount;
    uint frameID;
    uint frameTime;
    uint time;
};

class MandelbrotApp {
  public:
    void run() {
        std::cout << "starting application\n";

        initVulkan();
        mainLoop();
        cleanup();
    }

  private:
    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double, std::chrono::seconds::period>>
        lastTimeDelta;

    uint32_t version = VK_MAKE_VERSION(0, 3, 0);
    std::shared_ptr<Instance> instance;
    std::shared_ptr<Window> window;

    std::shared_ptr<PhysicalDevice> physicalDevice;
    std::shared_ptr<Device> device;

    std::shared_ptr<Queue> graphicsQueue;
    std::shared_ptr<Queue> presentQueue;

    std::shared_ptr<SwapChain> swapChain;
    std::shared_ptr<FrameBuffer> frameBuffer;
    VkExtent2D swapChainExtent;

    std::shared_ptr<RenderPass> renderPass;
    std::shared_ptr<DescriptorSetLayout> descriptorSetsLayout;

    std::shared_ptr<PipelineLayout> shadingPipelineLayout;
    std::shared_ptr<GraphicsPipeline> shadingPipeline;

    std::shared_ptr<CommandPool> commandPool;

    std::vector<std::shared_ptr<UniformBuffer<MandelbrotUBO>>> uniformBuffers;

    std::vector<std::shared_ptr<CommandBuffer>> graphicsCommandBuffers;

    std::array<float, 60> lastFrameDeltas;
    size_t currentFrame = 0;
    size_t currentFrameIndex = 0;
    float currentFPS = 0;

    bool framebufferResized = false;
    uint32_t imageCount;

    std::shared_ptr<DescriptorPool> graphicsDescriptorPool;
    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
    std::vector<std::shared_ptr<Fence>> imagesInFlight;

    //dexode::EventBus::Listener listener{publicRenderBus.getBus()};

    void initWindow() {
        Window::initWindows();
        window = Window::create(instance, "API Test " + std::to_string(version), WIDTH, HEIGHT);
    }

    void initVulkan() {
        if (debugLevel > 0) { std::cout << "initializing vulkan\n"; }

        createInstance();
        initWindow();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        queryDeviceProperties(physicalDevice);
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayouts();
        createGraphicsPipeline();
        createFrameBuffers();
        createCommandPool();
        createUniformBuffers();
        createGraphicsDescriptorPools();
        createGraphicsDescriptorSets();
        createGraphicsCommandBuffers();
        createSyncObjects();
        //submitEventBusFunctions();
    }

    /*void submitEventBusFunctions() {
        listener.listen<PreRenderUpdateBuffersEvent>([&](const auto &event) {
          updateWindowSize();
          updateUniformBuffer(swapChain->getCurrentImageIndex());
          currentFPS = getAverageFramerate();
        });

        listener.listen<RenderSubmitQueueEvent>([&](const auto &event) {
          std::vector<std::shared_ptr<Semaphore>> signalSemaphores = swapChain->getRenderSignalSemaphores();
          std::vector<std::shared_ptr<Semaphore>> waitSemaphores = swapChain->getRenderWaitSemaphores();

          graphicsCommandBuffers[swapChain->getCurrentImageIndex()]->submitToQueue(signalSemaphores,
                                                                            { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
                                                                            waitSemaphores,
                                                                            graphicsQueue,
                                                                            swapChain->getPresentFence());
        });

        publicRenderBus.getBus()->process();
    }*/

    void mainLoop() {
        if (debugLevel > 0) { std::cout << "entering main loop\n"; }

        while (!glfwWindowShouldClose(window->getWindow())) {
            glfwPollEvents();

            //ImGui_ImplVulkan_NewFrame();

            //ImGui_ImplGlfw_NewFrame();

            //ImGui::NewFrame();

            //ImGui::ShowDemowindow_();
            //setupImGUIFrame();
            drawFrame();

            //publicRenderBus.runRenderLoop(device);
        }

        device->waitIdle();

        if (debugLevel > 0) { std::cout << "exiting\n"; }
    }

    static void cleanup() {
        if (debugLevel > 0) { std::cout << "cleaning up\n"; }

        glfwTerminate();
    }

    void recreateSwapChain() {
        /*int width = 0, height = 0;
        glfwGetFramebufferSize(window_, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window_, &width, &height);
            glfwWaitEvents();
        }*/

        vkDeviceWaitIdle(device->getHandle());

        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFrameBuffers();
        createUniformBuffers();
        createGraphicsDescriptorPools();
        createGraphicsDescriptorSets();
        createGraphicsCommandBuffers();
    }

    void createInstance() {
        glfwInit();

        instance = Instance::create("api_test", version, true);
    }

    void setupDebugMessenger() {}

    void createSurface() {}

    void pickPhysicalDevice() {
        physicalDevice = instance->getPhysicalDevice()[0];
    }

    static void queryDeviceProperties(const std::shared_ptr<PhysicalDevice> &device) {
        device->queryDeviceProperties();
    }

    void createLogicalDevice() {
        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.shaderFloat64 = VK_TRUE;

        auto familyHandler = physicalDevice->getQueueFamilyHandler();

        optional<uint32_t> graphicsFamily = familyHandler->getOptimalQueueFamily(VK_QUEUE_GRAPHICS_BIT);
        if (graphicsFamily.has_value())
            familyHandler->registerQueueFamilyAllocation(graphicsFamily.value(), 1);

        optional<uint32_t> presentFamily = familyHandler->getOptimalPresentQueue(window);
        if (presentFamily.has_value())
            familyHandler->registerQueueFamilyAllocation(presentFamily.value(), 1);

        std::vector<uint32_t> swapChainQueues = familyHandler->getRegisteredQueueFamilies();

        device = Device::create(physicalDevice, deviceExtensions, deviceFeatures);

        presentQueue = physicalDevice->getQueueFamilyHandler()->getQueueFamily(presentFamily.value())->getQueue(0);
        graphicsQueue = physicalDevice->getQueueFamilyHandler()->getQueueFamily(graphicsFamily.value())->getQueue(0);

        std::cout << "Graphics queue: " << graphicsQueue->getHandle() << std::endl;
        std::cout << "Present queue: " << presentQueue->getHandle() << std::endl;
    }

    template<typename type>
    static void remove(std::vector<type> &v) {
        std::unordered_set<type> s;
        auto end = std::copy_if(v.begin(), v.end(), v.begin(), [&s](type const &i) {
          return s.insert(i).second;
        });

        v.erase(end, v.end());
    }

    void createSwapChain() {
        std::vector<uint32_t>
            accessingQueues = {graphicsQueue->getQueueFamilyIndex(), presentQueue->getQueueFamilyIndex()};
        remove(accessingQueues);

        swapChain = SwapChain::create(device, window, presentQueue, imageCount, accessingQueues);

        imageCount = swapChain->getImageCount();

        VkExtent2D extent = swapChain->getSwapExtent();

        swapChainExtent = extent;
    }

    void createImageViews() {
    }

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlag) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = aspectFlag;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(device->getHandle(), &createInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image view!");
        }

        return imageView;
    }

    void createRenderPass() {
        renderPass = RenderPass::create(device);

        uint32_t swapChainAttachment = renderPass->submitSwapChainAttachment(swapChain, false);

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = swapChainAttachment;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        std::vector<VkAttachmentReference> shadingPipelineColorReference = {colorAttachmentRef};
        std::vector<VkAttachmentReference> shadingPipelineAttachmentReference = { };
        uint32_t shadingSubpass =
            renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                      shadingPipelineColorReference,
                                      shadingPipelineAttachmentReference,
                                      nullptr,
                                      0);

        renderPass->submitDependency(VK_SUBPASS_EXTERNAL, shadingSubpass, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

        renderPass->build();
    }

    void createDescriptorSetLayouts() {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(1);

        setLayoutBindings[0].binding = 0;
        setLayoutBindings[0].descriptorCount = 1;
        setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        setLayoutBindings[0].pImmutableSamplers = nullptr;
        setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        descriptorSetsLayout = DescriptorSetLayout::create(device, setLayoutBindings);
    }

    void createGraphicsPipeline() {

        auto quadShaderStage = VertexShader::create(device, "main", "resources/shaders/compiled/quad.vert.spv");
        auto shadingShaderStage = FragmentShader::create(device, "main", "resources/shaders/compiled/uv.frag.spv");

        std::vector<std::shared_ptr<GraphicsShader>> shaders = {quadShaderStage, shadingShaderStage};
        std::vector<VkPipelineShaderStageCreateInfo>
            stages = {quadShaderStage->pipelineStageInfo(), shadingShaderStage->pipelineStageInfo()};

        std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts = {descriptorSetsLayout};

        shadingPipelineLayout = PipelineLayout::create(device, descriptorSetLayouts);
        VkExtent2D extent = {swapChainExtent.width, swapChainExtent.height};

        shadingPipeline =
            GraphicsPipeline::create(device,
                                     shadingPipelineLayout,
                                     shaders,
                                     renderPass,
                                     extent,
                                     VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
                                     VK_CULL_MODE_NONE);
    }

    void createFrameBuffers() {
        std::vector<std::vector<std::shared_ptr<ImageView>>> imageViews(imageCount,
                                                                        std::vector<std::shared_ptr<ImageView>>());

        frameBuffer = FrameBuffer::create(device, renderPass, swapChain, imageViews);
    }

    void createCommandPool() {
        commandPool = CommandPool::create(device, graphicsQueue, 0);
    }

    void createGraphicsDescriptorPools() {
        VkDescriptorPoolSize graphicsPoolSize{};
        graphicsPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        graphicsPoolSize.descriptorCount = static_cast<uint32_t>(imageCount);

        std::vector<VkDescriptorPoolSize> descriptorPoolSizes = { graphicsPoolSize };

        graphicsDescriptorPool = DescriptorPool::create(device, descriptorPoolSizes);
    }

    void createGraphicsDescriptorSets() {
        std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts(imageCount, descriptorSetsLayout);

        descriptorSets = DescriptorSet::create(descriptorSetLayouts, graphicsDescriptorPool);

        for (size_t i = 0; i < imageCount; i++) {
            descriptorSets[i]->updateUniformBuffer(uniformBuffers[i], 0);
        }
    }

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features) {
        for (VkFormat format: candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice->getHandle(), format, &props);

            if ((tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
                || (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    void createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(MandelbrotUBO);

        uniformBuffers.resize(imageCount);

        for (size_t i = 0; i < imageCount; i++) {
            uniformBuffers[i] = UniformBuffer<MandelbrotUBO>::create(device, graphicsQueue);
        }
    }

    void createGraphicsCommandBuffers() {
        graphicsCommandBuffers.resize(imageCount);

        for (uint32_t i = 0; i < graphicsCommandBuffers.size(); i++) {
            graphicsCommandBuffers[i] = CommandBuffer::create(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            graphicsCommandBuffers[i]->beginCommandBuffer();

            std::vector<VkClearValue> clearColor = {{{{0.0f, 0.0f, 0.0f, 1.0f}}}};

            graphicsCommandBuffers[i]->beginRenderPass(renderPass, frameBuffer, clearColor, i, swapChainExtent, {0, 0});

            std::vector<std::shared_ptr<DescriptorSet>> currentSets = {descriptorSets[i]};

            graphicsCommandBuffers[i]->bindPipeline(shadingPipeline);

            graphicsCommandBuffers[i]->bindDescriptorSets(currentSets, shadingPipeline);

            graphicsCommandBuffers[i]->draw(4, 1, 0, 0);

            //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), graphicsCommandBuffers[i]);

            graphicsCommandBuffers[i]->endRenderPass();

            graphicsCommandBuffers[i]->endCommandBuffer();
        }
    }

    void createSyncObjects() {
        imagesInFlight.resize(imageCount);
    }

    void drawFrame() {
        swapChain->acquireNextFrame({device});
        //ImGui::Render();

        updateWindowSize();
        updateUniformBuffer(swapChain->getCurrentImageIndex());
        currentFPS = getAverageFramerate();

        std::vector<std::shared_ptr<Semaphore>> signalSemaphores = swapChain->getRenderSignalSemaphores();
        std::vector<std::shared_ptr<Semaphore>> waitSemaphores = swapChain->getRenderWaitSemaphores();
        graphicsCommandBuffers[swapChain->getCurrentImageIndex()]->submitToQueue(signalSemaphores,
                                                                                 { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
                                                                                 waitSemaphores,
                                                                                 graphicsQueue,
                                                                                 swapChain->getPresentFence());

        swapChain->presentImage({device});
    }

    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - startTime).count();
        double
            timeDelta =
            std::chrono::duration<double, std::chrono::nanoseconds::period>(currentTime - lastTimeDelta).count();
        lastTimeDelta = currentTime;

        MandelbrotUBO ubo{};
        ubo.frameCount = static_cast<uint>(currentFrameIndex);
        ubo.frameID = ubo.frameCount;
        ubo.frameTime = static_cast<uint>(timeDelta);
        ubo.time = static_cast<uint>(time);

        currentFrameIndex++;

        uniformBuffers[currentImage]->getBuffer()->transferDataMapped(&ubo);
    }

    void updateWindowSize() {
        //glfwGetWindowSize(window_, &window_Height, &window_Width);
    }

    float getAverageFramerate() {
        float average = 0.0f;
        for (float value: lastFrameDeltas)
            average += value;
        return average / static_cast<float>(lastFrameDeltas.size());
    }
};

int main() {
    MandelbrotApp app;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "application closed\n";

    return EXIT_SUCCESS;
}
