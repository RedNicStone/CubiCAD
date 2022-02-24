//
// Created by nic on 28/01/2022.
//

#pragma once

#ifndef CUBICAD_RENDERMANAGER_H
#define CUBICAD_RENDERMANAGER_H

#include "framebufferselector.h"
#include "camera.h"
#include "materiallibrary.h"
#include "texturelibrary.h"
#include "meshlibrary.h"
#include "ui/uirenderer.h"
#include "scene.h"
#include "scenewriter.h"
#include "../utils/utils.h"
#include "shadingpipeline.h"


class SceneWriter;

enum RenderTarget : uint32_t {
    RENDER_TARGET_DEFAULT       = 0,
    RENDER_TARGET_DIFFUSE       = 0,
    RENDER_TARGET_POSITION      = 1,
    RENDER_TARGET_NORMAL        = 2,
    RENDER_TARGET_DEPTH         = 3,
    RENDER_TARGET_MAX           = 4
};

class RenderManager : public std::enable_shared_from_this<RenderManager> {
  private:
    // external objects
    std::shared_ptr<Instance> instance;
    std::shared_ptr<Window> window;
    uint32_t imageCount;

    // constant objects
    std::shared_ptr<PhysicalDevice> physicalDevice;
    std::shared_ptr<Device> device;

    std::shared_ptr<Queue> graphicsQueue;
    std::shared_ptr<Queue> transferQueue;
    std::shared_ptr<Queue> computeQueue;
    std::shared_ptr<Queue> presentQueue;

    std::shared_ptr<CommandPool> graphicsPool;
    std::shared_ptr<CommandPool> transferPool;
    std::shared_ptr<CommandPool> computePool;
    std::shared_ptr<CommandPool> presentPool;

    std::shared_ptr<CommandBuffer> drawCommandBuffer;

    std::shared_ptr<DescriptorSet> viewportDescriptor;
    std::shared_ptr<UniformBuffer> viewportUniform;

    // scene related objects
    CameraModel cameraModel;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<UIRenderer> uiRenderer;

    std::shared_ptr<Scene> scene;
    std::shared_ptr<SceneWriter> sceneWriter;

    std::shared_ptr<DescriptorPoolManager> poolManager;
    TextureQualitySettings textureQualitySettings;

    std::shared_ptr<MaterialLibrary> materialLibrary;
    std::shared_ptr<TextureLibrary> textureLibrary;
    std::shared_ptr<MeshLibrary> meshLibrary;

    std::shared_ptr<MasterMaterial> defaultMaterial;

    // objects affected by resize
    std::shared_ptr<SwapChain> swapChain;
    std::shared_ptr<FrameBuffer> frameBuffer;
    VkExtent2D swapChainExtent;

    std::shared_ptr<RenderPass> renderPass;

    uint32_t shadingSubpass;
    uint32_t uiSubpass;

    std::vector<std::shared_ptr<Image>> renderTargets;
    std::vector<std::shared_ptr<ImageView>> renderTargetViews;

    std::shared_ptr<Image> depthImage;
    std::shared_ptr<ImageView> depthImageView;

    std::shared_ptr<FramebufferSelector> objectBuffer;
    std::shared_ptr<Image> objectBufferImage;
    std::shared_ptr<ImageView> objectBufferImageView;

    std::shared_ptr<ShadingPipeline> viewportSelector;

    // input objects
    bool mouseCaptured = false;
    const float mouseSpeed = 0.001f;

    RenderTarget activePresentTarget = RENDER_TARGET_DEFAULT;

    // creation procedures
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPools();
    void createSwapChain();
    void createRenderPass();
    void createFrameBuffer();
    void createSceneObjects();
    void createPostProcessingPipelines();
    void createDefaultMaterial();
    void createUIObjects();

    void createRenderObjects();
    void recreateSwapChain(bool newImageCount);

    // other procedures
    void processMouseInputs();

    // callbacks
    static void keyCallback(GLFWwindow *glfwWindow, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow *glfwWindow, int button, int action, int mods);

  public:
    static std::shared_ptr<RenderManager> create(const std::shared_ptr<Instance> &instance,
                                                 const std::shared_ptr<Window> &window,
                                                 const TextureQualitySettings &textureQuality,
                                                 const CameraModel &cameraModel);

    void resizeSwapChain(uint32_t newImageCount = 0);

    void updateRenderData();
    void drawFrame();
    void processInputs();

    void loadMesh(const std::string &filename);

    std::shared_ptr<Scene> getScene() { return scene; }
    std::shared_ptr<SceneWriter> getSceneWriter() { return sceneWriter; }
    std::shared_ptr<MaterialLibrary> getMaterialLibrary() { return materialLibrary; }
    std::shared_ptr<TextureLibrary> getTextureLibrary() { return textureLibrary; }
    std::shared_ptr<MeshLibrary> getMeshLibrary() { return meshLibrary; }
    std::shared_ptr<UIRenderer> getUIRenderer() { return uiRenderer; }
    std::shared_ptr<Device> getDevice() { return device; }
    std::shared_ptr<RenderPass> getRenderPass() { return renderPass; }
    std::shared_ptr<DescriptorPoolManager> getDescriptorManager() { return poolManager; }
    std::shared_ptr<MasterMaterial> getDefaultMaterial() { return defaultMaterial; }

    VkExtent2D getExtend() { return swapChainExtent; }

    ~RenderManager();
};

#endif //CUBICAD_RENDERMANAGER_H
