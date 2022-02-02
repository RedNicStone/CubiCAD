//
// Created by nic on 28/01/2022.
//

#include "rendermanager.h"


std::shared_ptr<RenderManager> RenderManager::create(const std::shared_ptr<Instance>& instance,
                                                     const std::shared_ptr<Window>& window,
                                                     const TextureQualitySettings& textureQuality,
                                                     const CameraModel& cameraModel) {
    auto renderManager = std::make_shared<RenderManager>();

    window->setUserPointer(renderManager.get());

    renderManager->instance = instance;
    renderManager->window = window;
    renderManager->textureQualitySettings = textureQuality;
    renderManager->cameraModel = cameraModel;

    renderManager->createRenderObjects();

    return renderManager;
}

void RenderManager::pickPhysicalDevice() {
    physicalDevice = instance->getPhysicalDevice()[0];  // todo make device selection "smart"
}

void RenderManager::createLogicalDevice() {
    const std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.shaderFloat64 = VK_TRUE;

    auto familyHandler = physicalDevice->getQueueFamilyHandler();

    optional<uint32_t> graphicsFamily = familyHandler->getOptimalQueueFamily(VK_QUEUE_GRAPHICS_BIT);
    if (graphicsFamily.has_value())
        familyHandler->appendQueueFamilyAllocations(graphicsFamily.value(), 1);

    optional<uint32_t> transferFamily = familyHandler->getOptimalQueueFamily(VK_QUEUE_TRANSFER_BIT);
    if (transferFamily.has_value())
        familyHandler->appendQueueFamilyAllocations(transferFamily.value(), 1);

    optional<uint32_t> computeFamily = familyHandler->getOptimalQueueFamily(VK_QUEUE_COMPUTE_BIT);
    if (computeFamily.has_value())
        familyHandler->appendQueueFamilyAllocations(computeFamily.value(), 1);

    optional<uint32_t> presentFamily = familyHandler->getOptimalPresentQueue(window);
    if (presentFamily.has_value())
        familyHandler->appendQueueFamilyAllocations(presentFamily.value(), 1);

    device = Device::create(physicalDevice, deviceExtensions, deviceFeatures);

    graphicsQueue = physicalDevice->getQueueFamilyHandler()->getQueueFamily(graphicsFamily.value())->getQueue(0);
    transferQueue = physicalDevice->getQueueFamilyHandler()->getQueueFamily(transferFamily.value())->getQueue(0);
    computeQueue = physicalDevice->getQueueFamilyHandler()->getQueueFamily(computeFamily.value())->getQueue(0);
    presentQueue = physicalDevice->getQueueFamilyHandler()->getQueueFamily(presentFamily.value())->getQueue(0);

    std::cout << "Graphics queue: " << graphicsQueue->getHandle() << std::endl;
    std::cout << "Transfer queue: " << graphicsQueue->getHandle() << std::endl;
    std::cout << "Compute queue: " << graphicsQueue->getHandle() << std::endl;
    std::cout << "Present queue: " << presentQueue->getHandle() << std::endl;
}

void RenderManager::createCommandPools() {
    graphicsPool = CommandPool::create(device, graphicsQueue, 0);
    transferPool = CommandPool::create(device, transferQueue, 0);
    computePool = CommandPool::create(device, computeQueue, 0);
    presentPool = CommandPool::create(device, presentQueue, 0);
}

void RenderManager::createSwapChain() {
    std::vector<uint32_t>
        renderPassQueues = { graphicsQueue->getQueueFamilyIndex(), presentQueue->getQueueFamilyIndex() };
    Utils::remove(renderPassQueues);

    std::vector<uint32_t>
        graphicsQueues = {graphicsQueue->getQueueFamilyIndex()};

    swapChain = SwapChain::create(device, window, presentQueue, 3, renderPassQueues);

    imageCount = swapChain->getImageCount();
    VkExtent2D extent = swapChain->getSwapExtent();
    swapChainExtent = extent;

    renderPassQueues = {graphicsQueue->getQueueFamilyIndex() };
    depthImage = Image::create(device, swapChain->getSwapExtent(), 1, VK_FORMAT_D32_SFLOAT,
                               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, renderPassQueues);
    depthImageView = ImageView::create(depthImage, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT);

    objectBufferImage = Image::create(device, swapChain->getSwapExtent(), 1, VK_FORMAT_R32_UINT,
                                      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                      graphicsQueues, VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    objectBufferImageView = ImageView::create(objectBufferImage, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT);
    objectBuffer = FramebufferSelector::create(device, graphicsQueue, extent);
}

void RenderManager::createRenderPass() {

    renderPass = RenderPass::create(device);

    uint32_t swapChainAttachment = renderPass->submitSwapChainAttachment(swapChain, true);
    uint32_t depthAttachment = renderPass->submitImageAttachment(depthImage->getFormat(), VK_SAMPLE_COUNT_1_BIT,
                                                                 VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                                 VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                                 VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                                 VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                                 VK_IMAGE_LAYOUT_UNDEFINED,
                                                                 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    uint32_t objectIDAttachment = renderPass->submitImageAttachment(objectBufferImage->getFormat(),
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

    std::vector<VkAttachmentReference> shadingPipelineColorReference = { colorAttachmentRef, objectIDAttachmentRef };
    std::vector<VkAttachmentReference> shadingPipelineAttachmentReference = { };
    renderSubpass =
        renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  shadingPipelineColorReference,
                                  shadingPipelineAttachmentReference,
                                  &depthAttachmentRef,
                                  0);

    std::vector<VkAttachmentReference> UIPipelineColorReference = { colorAttachmentRef };
    std::vector<VkAttachmentReference> UIPipelineAttachmentReference = { };
    uiSubpass =
        renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  UIPipelineColorReference,
                                  UIPipelineAttachmentReference,
                                  nullptr,
                                  0);

    renderPass->submitDependency(VK_SUBPASS_EXTERNAL, renderSubpass, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    renderPass->submitDependency(renderSubpass, uiSubpass, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    renderPass->build();
}

void RenderManager::createFrameBuffer() {
    std::vector<std::vector<std::shared_ptr<ImageView>>> imageViews(imageCount,
                                                                    std::vector<std::shared_ptr<ImageView>>{
                                                                        depthImageView,
                                                                        objectBufferImageView
                                                                    });

    frameBuffer = FrameBuffer::create(device, renderPass, swapChain, imageViews);
}

void RenderManager::crateSceneObjects() {
    camera = Camera::create(cameraModel, swapChainExtent);
    scene = Scene::create(device, transferQueue, graphicsQueue, camera);
    auto currentDir = std::string(get_current_dir_name());
    sceneWriter = SceneWriter::create(shared_from_this(), currentDir);

    poolManager = DescriptorPoolManager::create(device);

    materialLibrary = MaterialLibrary::create(device, transferPool, poolManager, { graphicsQueue });
    textureLibrary = TextureLibrary::create(device, graphicsQueue, graphicsPool, textureQualitySettings);
    meshLibrary = MeshLibrary::create();
}

void RenderManager::crateUIObjects() {
    uiRenderer = UIRenderer::create(graphicsQueue, graphicsPool, renderPass, window, imageCount, uiSubpass);
}

void RenderManager::createRenderObjects() {
    glfwSetMouseButtonCallback(window->getWindow(), mouseButtonCallback);
    glfwSetKeyCallback(window->getWindow(), keyCallback);

    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPools();
    createSwapChain();
    createRenderPass();
    createFrameBuffer();
    crateSceneObjects();
    crateUIObjects();
}

void RenderManager::recreateSwapChain(bool newImageCount) {
    createSwapChain();
    createRenderPass();
    createFrameBuffer();
    camera->updateCameraModel(cameraModel, swapChainExtent);

    if (newImageCount) {
        crateUIObjects();
    }
}

void RenderManager::processMouseInputs() {
    if (glfwGetWindowAttrib(window->getWindow(), GLFW_HOVERED) == GLFW_TRUE) {
        auto& io = UIRenderer::getIO();

        if (!mouseCaptured && !io.WantCaptureMouse) {
            VkExtent2D mousePos { static_cast<uint32_t>(io.MousePos.x), static_cast<uint32_t>(io.MousePos.y) };
            uint32_t objectID = objectBuffer->getIDAtPosition(mousePos);

            scene->setHovered(objectID);
        } else if (io.WantCaptureMouse) {
            scene->setHovered(0);
        } else if (mouseCaptured) {
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

            camera->rotate(io.MouseDelta.x * mouseSpeed, {0, -1, 0});
            camera->rotate(io.MouseDelta.y * mouseSpeed,
                           glm::normalize(glm::cross({0, 1, 0}, camera->getRotation())));
        }
    }
}

void RenderManager::resizeSwapChain(uint32_t newImageCount) {
    bool bNewImageCount = newImageCount != 0 && newImageCount != imageCount;
    if (bNewImageCount)
        imageCount = newImageCount;
    recreateSwapChain(newImageCount);
}

void RenderManager::drawFrame() {
    // acquire frame
    swapChain->acquireNextFrame();
    uint32_t index = swapChain->getCurrentImageIndex();

    // allocate command buffer
    drawCommandBuffer = CommandBuffer::create(graphicsPool);

    std::vector<std::shared_ptr<Semaphore>> signalSemaphores = swapChain->getRenderSignalSemaphores();
    std::vector<std::shared_ptr<Semaphore>> waitSemaphores = swapChain->getRenderWaitSemaphores();

    // begin recording instructions
    drawCommandBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // clear and begin render pass
    std::vector<VkClearValue> clearColor(3);
    clearColor[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearColor[1].depthStencil = {1.0, 0};
    clearColor[2].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    drawCommandBuffer->beginRenderPass(renderPass, frameBuffer, clearColor,
                                                   index, frameBuffer->getExtent(), {0, 0});

    // write all commands to draw the scene
    scene->bakeGraphicsBuffer(drawCommandBuffer);

    // begin next subpass
    drawCommandBuffer->nextSubpass();

    // write all commands to draw the UI
    uiRenderer->draw(drawCommandBuffer);

    // end the render pass
    drawCommandBuffer->endRenderPass();

    // transition the object buffer into a layout we can copy from
    objectBufferImage->setLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    objectBufferImage->transitionImageLayout(drawCommandBuffer,
                                             VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                                             VK_ACCESS_TRANSFER_READ_BIT);

    // transition the CPU sided object buffer into a layout we can copy to
    objectBuffer->transferLayoutWrite(drawCommandBuffer);

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

    // copy GPU sided object buffer to the CPU sided object buffer
    // todo? only copy region of interest ?
    drawCommandBuffer->copyImage(objectBufferImage, objectBuffer->getImage(), copyRegions);

    // transition the object buffer into a layout we can write to from the fragment shader
    objectBufferImage->transitionImageLayout(drawCommandBuffer,
                                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                             VK_ACCESS_SHADER_WRITE_BIT);

    // transition the CPU sided object buffer into a layout we can read from
    objectBuffer->transferLayoutRead(drawCommandBuffer);

    // compile all commands [resource intensive part]
    drawCommandBuffer->endCommandBuffer();

    // submit the buffer for execution
    drawCommandBuffer->submitToQueue(signalSemaphores, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
                                                 waitSemaphores,
                                                 graphicsQueue,
                                                 swapChain->getPresentFence());

    // present the image and advance swap chain
    swapChain->presentImage();
}

void RenderManager::processInputs() {
    processMouseInputs();
    scene->updateUBO();
}

void RenderManager::keyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    auto renderManager =
        static_cast<RenderManager*>(static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))->getUserPointer());
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !UIRenderer::getIO().WantCaptureKeyboard) {
        if (renderManager->mouseCaptured)
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        renderManager->mouseCaptured =! renderManager->mouseCaptured;
        renderManager->uiRenderer->setHidden(renderManager->mouseCaptured);
    }
}

void RenderManager::mouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
    auto renderManager =
        static_cast<RenderManager*>(static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))->getUserPointer());
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !UIRenderer::getIO().WantCaptureMouse)
        renderManager->scene->setSelected(renderManager->scene->getHovered());
}

RenderManager::~RenderManager() {
    graphicsQueue->waitForIdle();
    transferQueue->waitForIdle();
    computeQueue->waitForIdle();
    presentQueue->waitForIdle();
    device->waitIdle();
}

