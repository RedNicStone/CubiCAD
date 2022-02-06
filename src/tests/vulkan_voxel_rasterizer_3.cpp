//
// Created by nic on 07/12/2021.
//

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>


#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

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
#include "../renderer/modelloader.h"
#include "../renderer/scene.h"
#include "../renderer/ui/uirenderer.h"
#include "../renderer/framebufferselector.h"
#include "../renderer/ui/objectproperties.h"
#include "../renderer/ui/objectlist.h"
#include "../renderer/ui/mainmenu.h"
#include "../renderer/texturelibrary.h"


const uint32_t WIDTH = 1080 * 2;
const uint32_t HEIGHT = 720 * 2;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

const uint32_t debugLevel = 3;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class MandelbrotApp {
  public:
    void run() {
        std::cout << "starting application\n";

        initVulkan();
        mainLoop();
        cleanup();
    }

  private:
    uint32_t version = VK_MAKE_VERSION(0, 3, 0);
    std::shared_ptr<Instance> instance;
    std::shared_ptr<Window> window;

    std::shared_ptr<PhysicalDevice> physicalDevice;
    std::shared_ptr<Device> device;

    std::shared_ptr<Queue> graphicsQueue;
    std::shared_ptr<Queue> presentQueue;

    std::shared_ptr<SwapChain> swapChain;
    std::shared_ptr<FrameBuffer> frameBuffer;
    VkExtent2D swapChainExtent{};

    std::shared_ptr<Image> depthImage;
    std::shared_ptr<ImageView> depthImageView;

    std::shared_ptr<RenderPass> renderPass;
    uint32_t UISubpass{};

    std::shared_ptr<CommandPool> commandPool;

    std::vector<std::shared_ptr<CommandBuffer>> graphicsCommandBuffers;

    std::shared_ptr<Scene> scene;
    std::shared_ptr<ModelLoader> modelLoader;
    std::shared_ptr<Mesh> model;
    std::vector<std::shared_ptr<MeshInstance>> objects;

    std::shared_ptr<Camera> camera;
    std::shared_ptr<FramebufferSelector> objectBuffer;
    std::shared_ptr<Image> objectBufferImage;
    std::shared_ptr<ImageView> objectBufferImageView;

    std::shared_ptr<UIRenderer> UI;
    std::shared_ptr<ObjectProperties> objectProperties;
    std::shared_ptr<ObjectList> objectList;
    std::shared_ptr<MainMenu> menuBar;
    std::shared_ptr<TextureLibrary> textureLibrary;

    bool mouseCaptured = false;
    uint32_t imageCount{};

    std::vector<std::shared_ptr<Fence>> imagesInFlight;

    glm::dvec2 prevMousePos{};
    bool mouseDown = false;
    bool escDown = false;

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
        createCommandPool();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createFrameBuffers();
        createGraphicsCommandBuffers();
        createSyncObjects();
        loadModels();
        createScene();
        createUI();
        //submitEventBusFunctions();
    }

    void createUI() {
        TextureQualitySettings settings{};
        settings.mipLevels = 1;
        settings.anisotropy = 0;
        textureLibrary = TextureLibrary::create(device, graphicsQueue, commandPool, settings);

        UI = UIRenderer::create(graphicsQueue, commandPool, renderPass, window, imageCount, UISubpass);
        objectProperties = ObjectProperties::create(scene);
        UI->submitDrawable(objectProperties);
        objectList = ObjectList::create(scene);
        UI->submitDrawable(objectList);
        menuBar = MainMenu::create(textureLibrary);
        UI->submitDrawable(menuBar);
    }

    void loadModels() {
        modelLoader = ModelLoader::create();

        auto vertexShader = VertexShader::create(device, "main", "resources/shaders/compiled/PBR_basic.vert.spv");
        auto fragmentShader = FragmentShader::create(device, "main", "resources/shaders/compiled/PBR_basic.frag.spv");
        std::vector<std::shared_ptr<GraphicsShader>> shaders{vertexShader, fragmentShader};

        MaterialProperty materialProperty{};
        materialProperty.input =
            static_cast<MaterialPropertyInput>(MATERIAL_PROPERTY_INPUT_CONSTANT | MATERIAL_PROPERTY_INPUT_TEXTURE);
        materialProperty.size = MATERIAL_PROPERTY_SIZE_8;
        materialProperty.count = MATERIAL_PROPERTY_COUNT_3;
        materialProperty.format = MATERIAL_PROPERTY_FORMAT_UINT;

        MaterialPropertyLayout propertyLayout{{materialProperty}};

        auto
            masterMaterial =
            MasterMaterial::create(device, shaders, 2, swapChainExtent, propertyLayout, renderPass, "basicMaterial");
        auto material = Material::create(masterMaterial);
        model = modelLoader->import("resources/models/demo/primitives/cube.obj", material).front();

        for (size_t i = 0; i < 5000; i++) {
            auto object = MeshInstance::create(model);
            object->setScale(glm::vec3(0.1f));
            object->setPosition(glm::normalize(glm::vec3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
                                                         static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
                                                         static_cast <float> (rand()) / static_cast <float> (RAND_MAX))
                                                   - glm::vec3(0.5f)) * glm::vec3(20));
            objects.push_back(object);
        }
    }

    void createScene() {
        CameraModel cameraModel{};
        cameraModel.cameraMode = CAMERA_MODE_PERSPECTIVE_INFINITE;
        cameraModel.fieldOfView = 75.0;
        cameraModel.clipNear = 0.1;
        cameraModel.clipFar = 100.0;

        camera = Camera::create(cameraModel, window->getSurfaceExtend());

        scene = Scene::create(device, graphicsQueue, graphicsQueue, camera);

        for (const auto &object: objects)
            scene->submitInstance(object);
        scene->collectRenderBuffers();
        scene->bakeMaterials(true);
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
            updateObjects();
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

    void updateObjects() {
        if (glfwGetWindowAttrib(window->getWindow(), GLFW_HOVERED) == GLFW_TRUE) {
            if (!mouseCaptured) {
                VkExtent2D mousePos{};
                mousePos.width = static_cast<uint32_t>(prevMousePos.x) + WIDTH;
                mousePos.height = static_cast<uint32_t>(prevMousePos.y) + HEIGHT;

                uint32_t objectID = objectBuffer->getIDAtPosition(mousePos);

                scene->setHovered(objectID);
            }

            auto &io = UIRenderer::getIO();

            glm::dvec2 currentMousePos;
            glfwGetCursorPos(window->getWindow(),
                             reinterpret_cast<double *>(&currentMousePos.x),
                             reinterpret_cast<double *>(&currentMousePos.y));
            currentMousePos -= glm::dvec2(WIDTH, HEIGHT);
            auto deltaMousePos = currentMousePos - prevMousePos;
            prevMousePos = currentMousePos;

            if (!escDown && glfwGetKey(window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                if (mouseCaptured) {
                    glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    mouseCaptured = false;
                    UI->setHidden(false);
                } else {
                    glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    mouseCaptured = true;
                    UI->setHidden(true);
                }
                escDown = true;
            } else if (escDown && glfwGetKey(window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE)
                escDown = false;

            if (!mouseCaptured && !io.WantCaptureMouse) {
                if (!mouseDown && glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                    scene->setSelected(scene->getHovered());
                    mouseDown = true;
                } else if (mouseDown && glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                    mouseDown = false;
            }

            if (mouseCaptured) {
                auto moveSpeed = 0.02f;
                if (glfwGetKey(window->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                    moveSpeed *= 3;
                }
                if (glfwGetKey(window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
                    camera->move(camera->getRotation() * glm::vec3(moveSpeed));
                }
                if (glfwGetKey(window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
                    camera->move(-camera->getRotation() * glm::vec3(moveSpeed));
                }
                if (glfwGetKey(window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
                    camera->move(-glm::cross(camera->getRotation(), {0, 1, 0}) * glm::vec3(moveSpeed));
                }
                if (glfwGetKey(window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
                    camera->move(glm::cross(camera->getRotation(), {0, 1, 0}) * glm::vec3(moveSpeed));
                }

                const auto mouseSpeed = 0.001f;
                camera->rotate((float) deltaMousePos.x * mouseSpeed, {0, -1, 0});
                camera->rotate((float) deltaMousePos.y * mouseSpeed,
                               glm::normalize(glm::cross({0, 1, 0}, camera->getRotation())));
            }
        }

        objectList->updateSelected();
        scene->updateUBO();

        for (const auto &object: objects) {
            object->setPosition(glm::normalize(glm::cross(object->getPosition(), {0, 1, 0}) * glm::vec3(0.0005f)
                                                   + object->getPosition()) * glm::vec3(20));
        }
        objectProperties->setObjectByID(scene->getSelected());
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
        createFrameBuffers();
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
            renderPassQueues = {graphicsQueue->getQueueFamilyIndex(), presentQueue->getQueueFamilyIndex()};
        remove(renderPassQueues);

        std::vector<uint32_t> graphicsQueues = {graphicsQueue->getQueueFamilyIndex()};

        swapChain = SwapChain::create(device, window, presentQueue, 3, renderPassQueues);

        renderPassQueues = {graphicsQueue->getQueueFamilyIndex()};
        depthImage =
            Image::create(device,
                          swapChain->getSwapExtent(),
                          1,
                          VK_FORMAT_D32_SFLOAT,
                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                          renderPassQueues);
        depthImageView = ImageView::create(depthImage, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT);

        objectBufferImage =
            Image::create(device,
                          swapChain->getSwapExtent(),
                          1,
                          VK_FORMAT_R32_UINT,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                          graphicsQueues,
                          VK_ACCESS_SHADER_WRITE_BIT,
                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        objectBufferImageView = ImageView::create(objectBufferImage, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT);

        objectBuffer = FramebufferSelector::create(device, graphicsQueue, window->getSurfaceExtend());

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

        uint32_t swapChainAttachment = renderPass->submitSwapChainAttachment(swapChain, true);
        uint32_t
            depthAttachment =
            renderPass->submitImageAttachment(depthImage->getFormat(),
                                              VK_SAMPLE_COUNT_1_BIT,
                                              VK_ATTACHMENT_LOAD_OP_CLEAR,
                                              VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                              VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                              VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                              VK_IMAGE_LAYOUT_UNDEFINED,
                                              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        uint32_t
            objectIDAttachment =
            renderPass->submitImageAttachment(objectBufferImage->getFormat(),
                                              VK_SAMPLE_COUNT_1_BIT,
                                              VK_ATTACHMENT_LOAD_OP_CLEAR,
                                              VK_ATTACHMENT_STORE_OP_STORE,
                                              VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                              VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                              VK_IMAGE_LAYOUT_UNDEFINED,
                                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = swapChainAttachment;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = depthAttachment;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference objectIDAttachmentRef{};
        objectIDAttachmentRef.attachment = objectIDAttachment;
        objectIDAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        std::vector<VkAttachmentReference> shadingPipelineColorReference = {colorAttachmentRef, objectIDAttachmentRef};
        std::vector<VkAttachmentReference> shadingPipelineAttachmentReference = {};
        uint32_t
            shadingSubpass =
            renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                      shadingPipelineColorReference,
                                      shadingPipelineAttachmentReference,
                                      &depthAttachmentRef,
                                      0);

        std::vector<VkAttachmentReference> UIPipelineColorReference = {colorAttachmentRef};
        std::vector<VkAttachmentReference> UIPipelineAttachmentReference = {};
        UISubpass =
            renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                      UIPipelineColorReference,
                                      UIPipelineAttachmentReference,
                                      nullptr,
                                      0);

        renderPass->submitDependency(VK_SUBPASS_EXTERNAL, shadingSubpass, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        renderPass->submitDependency(shadingSubpass, UISubpass, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

        renderPass->build();
    }

    void createFrameBuffers() {
        std::vector<std::vector<std::shared_ptr<ImageView>>>
            imageViews(imageCount, std::vector<std::shared_ptr<ImageView>>{depthImageView, objectBufferImageView});

        frameBuffer = FrameBuffer::create(device, renderPass, swapChain, imageViews);
    }

    void createCommandPool() {
        commandPool = CommandPool::create(device, graphicsQueue, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    }

    void createGraphicsCommandBuffers() {
        graphicsCommandBuffers.resize(imageCount);

        for (auto &graphicsCommandBuffer: graphicsCommandBuffers) {
            graphicsCommandBuffer = CommandBuffer::create(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        }
    }

    void createSyncObjects() {
        imagesInFlight.resize(imageCount);
    }

    void drawFrame() {
        glfwPollEvents();

        swapChain->acquireNextFrame({device});
        //ImGui::Render();
        uint32_t index = swapChain->getCurrentImageIndex();

        updateWindowSize();

        std::vector<std::shared_ptr<Semaphore>> signalSemaphores = swapChain->getRenderSignalSemaphores();
        std::vector<std::shared_ptr<Semaphore>> waitSemaphores = swapChain->getRenderWaitSemaphores();

        graphicsCommandBuffers[index]->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        std::vector<VkClearValue> clearColor(3);
        clearColor[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearColor[1].depthStencil = {1.0, 0};
        clearColor[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        graphicsCommandBuffers[index]->beginRenderPass(renderPass,
                                                       frameBuffer,
                                                       clearColor,
                                                       index,
                                                       frameBuffer->getExtent(),
                                                       {0, 0});

        scene->bakeGraphicsBuffer(graphicsCommandBuffers[index]);

        graphicsCommandBuffers[index]->nextSubpass();

        UI->draw(graphicsCommandBuffers[index]);

        graphicsCommandBuffers[index]->endRenderPass();

        objectBufferImage->setLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        objectBufferImage->transitionImageLayout(graphicsCommandBuffers[index],
                                                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                 VK_ACCESS_TRANSFER_READ_BIT);

        objectBuffer->transferLayoutWrite(graphicsCommandBuffers[index]);

        VkImageSubresourceLayers imageLayers{};
        imageLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageLayers.baseArrayLayer = 0;
        imageLayers.layerCount = 1;
        imageLayers.mipLevel = 0;

        std::vector<VkImageCopy> copyRegions(1);
        copyRegions[0].extent = {swapChainExtent.width, swapChainExtent.height, 1};
        copyRegions[0].srcOffset = {0, 0, 0};
        copyRegions[0].dstOffset = {0, 0, 0};
        copyRegions[0].srcSubresource = imageLayers;
        copyRegions[0].dstSubresource = imageLayers;

        graphicsCommandBuffers[index]->copyImage(objectBufferImage, objectBuffer->getImage(), copyRegions);

        objectBufferImage->transitionImageLayout(graphicsCommandBuffers[index],
                                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                 VK_ACCESS_SHADER_WRITE_BIT);

        objectBuffer->transferLayoutRead(graphicsCommandBuffers[index]);

        graphicsCommandBuffers[index]->endCommandBuffer();

        graphicsCommandBuffers[index]->submitToQueue(signalSemaphores,
                                                     {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
                                                     waitSemaphores,
                                                     graphicsQueue,
                                                     swapChain->getPresentFence());

        swapChain->presentImage({device});
    }

    void updateWindowSize() {
        //glfwGetWindowSize(window_, &window_Height, &window_Width);
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
