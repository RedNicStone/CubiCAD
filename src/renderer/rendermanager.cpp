//
// Created by nic on 28/01/2022.
//

#include "rendermanager.h"


std::shared_ptr<RenderManager> RenderManager::create(const std::shared_ptr<Instance> &instance,
                                                     const std::shared_ptr<Window> &window,
                                                     const TextureQualitySettings &textureQuality,
                                                     const CameraModel &cameraModel) {
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
    for (const auto& possiblePhysicalDevice : instance->getPhysicalDevices()) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(possiblePhysicalDevice->getHandle(), &deviceProperties);

        std::string name = deviceProperties.deviceName;
        std::cout << " * " << name;
        if (name.find("llvmpipe") == std::string::npos) { // check if device is virtual
            physicalDevice = possiblePhysicalDevice;
            std::cout << "  [USING]";
        }
        std::cout << std::endl;

    }
}

void RenderManager::createLogicalDevice() {
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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
        renderPassQueues = {graphicsQueue->getQueueFamilyIndex(), presentQueue->getQueueFamilyIndex()};
    Utils::remove(renderPassQueues);

    std::vector<uint32_t> graphicsQueues = {graphicsQueue->getQueueFamilyIndex()};

    swapChain = SwapChain::create(device, window, presentQueue, 3, renderPassQueues);

    imageCount = swapChain->getImageCount();
    VkExtent2D extent = swapChain->getSwapExtent();
    swapChainExtent = extent;

    renderTargets.clear();
    renderTargetViews.clear();
    renderTargets.resize(RENDER_TARGET_MAX);
    renderTargetViews.resize(RENDER_TARGET_MAX);
    for (uint32_t i = 0; i < RENDER_TARGET_MAX; i++) {
        VkFormat imageFormat;
        if (i == RENDER_TARGET_DEPTH)
            imageFormat = VK_FORMAT_D32_SFLOAT;
        else if (i == RENDER_TARGET_NORMAL)
            imageFormat = VK_FORMAT_R8G8B8_UNORM;

        VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        if (i == RENDER_TARGET_DEPTH)
            imageUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        else
            imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        renderTargets[i] = Image::create(device,
                                         swapChain->getSwapExtent(),
                                         1,
                                         imageFormat,
                                         imageUsage,
                                         graphicsQueues);

        VkImageAspectFlags imageAspect = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (i == RENDER_TARGET_DEPTH)
            imageAspect |= VK_IMAGE_ASPECT_DEPTH_BIT;

        renderTargetViews[i] = ImageView::create(renderTargets[i], VK_IMAGE_VIEW_TYPE_2D, imageAspect);
    }

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
    objectBuffer = FramebufferSelector::create(device, graphicsQueue, extent);
}

void RenderManager::createRenderPass() {
    renderPass = RenderPass::create(device);

    uint32_t swapChainAttachment = renderPass->submitSwapChainAttachment(swapChain, true);

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

    std::vector<uint32_t> attachments(RENDER_TARGET_MAX);
    std::vector<VkAttachmentReference> attachmentReferences(RENDER_TARGET_MAX);
    for (uint32_t i = 0; i < RENDER_TARGET_MAX; i++) {
        attachments.push_back(renderPass->submitImageAttachment(renderTargets[i]->getFormat(),
                                                                VK_SAMPLE_COUNT_1_BIT,
                                                                VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                                VK_ATTACHMENT_STORE_OP_STORE,
                                                                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                                VK_IMAGE_LAYOUT_UNDEFINED,
                                                                VK_IMAGE_LAYOUT_UNDEFINED));

        attachmentReferences[i].attachment = attachments[i];
        attachmentReferences[i].layout = renderTargets[i]->getLayout();
    }

    VkAttachmentReference swapChainAttachmentRef{};
    swapChainAttachmentRef.attachment = swapChainAttachment;
    swapChainAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference objectIDAttachmentRef{};
    objectIDAttachmentRef.attachment = objectIDAttachment;
    objectIDAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    std::vector<VkAttachmentReference> geometryPipelineColorReference(attachmentReferences);
    geometryPipelineColorReference.push_back(objectIDAttachmentRef);
    std::vector<VkAttachmentReference> geometryPipelineInputReference = {};
    uint32_t geometrySubpass =
        renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  geometryPipelineColorReference,
                                  geometryPipelineInputReference,
                                  &attachmentReferences[RENDER_TARGET_DEPTH],
                                  0);

    std::vector<VkAttachmentReference> shadingPipelineColorReference = {swapChainAttachmentRef};
    std::vector<VkAttachmentReference> shadingPipelineInputReference(attachmentReferences);
    uint32_t shadingSubpass =
        renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  shadingPipelineColorReference,
                                  shadingPipelineInputReference,
                                  nullptr,
                                  0);

    std::vector<VkAttachmentReference> UIPipelineColorReference = {swapChainAttachmentRef};
    std::vector<VkAttachmentReference> UIPipelineInputReference = {};
    uiSubpass =
        renderPass->submitSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  UIPipelineColorReference,
                                  UIPipelineInputReference,
                                  nullptr,
                                  0);

    renderPass->submitDependency(VK_SUBPASS_EXTERNAL, geometrySubpass, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    renderPass->submitDependency(geometrySubpass, shadingSubpass, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                                 VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    renderPass->submitDependency(geometrySubpass, uiSubpass, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    renderPass->build();
}

void RenderManager::createFrameBuffer() {
    std::vector<std::shared_ptr<ImageView>> frameViews(RENDER_TARGET_MAX);
    for (uint32_t i = 0; i < RENDER_TARGET_MAX; i++)
        frameViews[i] = renderTargetViews[i];

    std::vector<std::vector<std::shared_ptr<ImageView>>> imageViews(imageCount, frameViews);

    frameBuffer = FrameBuffer::create(device, renderPass, swapChain, imageViews);
}

void RenderManager::createSceneObjects() {
    camera = Camera::create(cameraModel, swapChainExtent);
    scene = Scene::create(device, transferQueue, graphicsQueue, camera);
    auto currentDir = std::string(get_current_dir_name());
    sceneWriter = SceneWriter::create(shared_from_this(), currentDir);

    poolManager = DescriptorPoolManager::create(device);

    textureLibrary = TextureLibrary::create(device, graphicsQueue, graphicsPool, textureQualitySettings);
    materialLibrary = MaterialLibrary::create(device, transferPool, poolManager, textureLibrary, {graphicsQueue});
    meshLibrary = MeshLibrary::create(textureLibrary, materialLibrary);
}

void RenderManager::createDefaultMaterial() {
    auto vertexShader = VertexShader::create(device, "main", "resources/shaders/compiled/"
                                                                                 "PBR_basic.vert.spv");
    auto fragmentShader = FragmentShader::create(device, "main", "resources/shaders/compiled/"
                                                                                     "PBR_basic.frag.spv");
    std::vector<std::shared_ptr<GraphicsShader>> shaders{vertexShader, fragmentShader};

    MaterialProperty materialProperty{};
    materialProperty.input =
        static_cast<MaterialPropertyInput>(MATERIAL_PROPERTY_INPUT_CONSTANT | MATERIAL_PROPERTY_INPUT_TEXTURE);
    materialProperty.size = MATERIAL_PROPERTY_SIZE_8;
    materialProperty.count = MATERIAL_PROPERTY_COUNT_4;
    materialProperty.format = MATERIAL_PROPERTY_FORMAT_SRGB;
    materialProperty.attributeName = "diffuse";

    MaterialPropertyLayout propertyLayout{{materialProperty}};
    auto layoutBuilt = buildLayout(propertyLayout);

    defaultMaterial =
        MasterMaterial::create(device,
                               shaders,
                               2,
                               swapChainExtent,
                               layoutBuilt,
                               renderPass,
                               poolManager,
                               "basicMaterial");
    defaultMaterial->updateImageSampler(textureLibrary);
}

void RenderManager::createUIObjects() {
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
    createSceneObjects();
    createDefaultMaterial();
    createUIObjects();
}

void RenderManager::recreateSwapChain(bool newImageCount) {
    createSwapChain();
    createRenderPass();
    createFrameBuffer();
    createDefaultMaterial();
    camera->updateCameraModel(cameraModel, swapChainExtent);

    if (newImageCount) {
        createUIObjects();
    }
}

void RenderManager::processMouseInputs() {
    if (glfwGetWindowAttrib(window->getWindow(), GLFW_HOVERED) == GLFW_TRUE) {
        auto &io = UIRenderer::getIO();

        if (!mouseCaptured && !io.WantCaptureMouse) {
            VkExtent2D mousePos{static_cast<uint32_t>(io.MousePos.x), static_cast<uint32_t>(io.MousePos.y)};
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
            camera->rotate(io.MouseDelta.y * mouseSpeed, glm::normalize(glm::cross({0, 1, 0}, camera->getRotation())));
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

    // clear and begin render pass (Geometry)
    std::vector<VkClearValue> clearColor(3);
    clearColor[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearColor[1].depthStencil = {1.0, 0};
    clearColor[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    drawCommandBuffer->beginRenderPass(renderPass, frameBuffer, clearColor, index, frameBuffer->getExtent(), {0, 0});

    // write all commands to draw the scene
    scene->bakeGraphicsBuffer(drawCommandBuffer);

    // begin next subpass (Shading)
    drawCommandBuffer->nextSubpass();

    // begin next subpass (UI)
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
    drawCommandBuffer->submitToQueue(signalSemaphores,
                                     {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
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

void RenderManager::keyCallback(GLFWwindow *glfwWindow, int key, int scancode, int action, int mods) {
    auto
        renderManager =
        static_cast<RenderManager *>(static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow))->getUserPointer());
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !UIRenderer::getIO().WantCaptureKeyboard) {
        if (renderManager->mouseCaptured)
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        renderManager->mouseCaptured = !renderManager->mouseCaptured;
        renderManager->uiRenderer->setHidden(renderManager->mouseCaptured);
    }
}

void RenderManager::mouseButtonCallback(GLFWwindow *glfwWindow, int button, int action, int mods) {
    auto
        renderManager =
        static_cast<RenderManager *>(static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow))->getUserPointer());
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

void RenderManager::updateRenderData() {
    materialLibrary->pushParameters();
    scene->bakeMaterials(true);
}

void RenderManager::loadMesh(const std::string &filename) {
    auto meshes = meshLibrary->createMesh(filename, defaultMaterial);
    for (const auto &mesh: meshes)
        scene->submitInstance(MeshInstance::create(mesh));
}

void RenderManager::createPostProcessingPipelines() {
    auto shader = FragmentShader::create(device, "main", "resources/shaders/viewport_selector.frag");

    std::shared_ptr<DescriptorSetLayout> viewportDescriptor = DescriptorSetLayout::create(device, );

    viewportSelector = ShadingPipeline::create(device, renderPass, shader, , swapChainExtent, 1);
}

