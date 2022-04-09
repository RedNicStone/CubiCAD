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
#include "ssaopipeline.h"


class SceneWriter;

class Scene;

/// Enum of available render targets
enum RenderTarget : uint32_t {
    RENDER_TARGET_DEFAULT = 0,
    RENDER_TARGET_DIFFUSE = 0,
    RENDER_TARGET_POSITION = 1,
    RENDER_TARGET_NORMAL = 2,
    RENDER_TARGET_DEPTH = 3,
    RENDER_TARGET_AMBIENT = 4,
    RENDER_TARGET_MAX = 5
};

/// Structure to store render configs
struct RenderQualityOptions {
    bool enableSSAO = false;
    uint32_t SSAOSampleCount = 16;
    float SSAOSampleRadius = 0.04f;

    uint32_t bufferCount = 1;
};

/// Main class used for all rendering actions
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
    RenderQualityOptions renderQuality;

    std::shared_ptr<MaterialLibrary> materialLibrary;
    std::shared_ptr<TextureLibrary> textureLibrary;
    std::shared_ptr<MeshLibrary> meshLibrary;

    std::shared_ptr<MasterMaterialTemplate> defaultMaterialTemplate;

    // objects affected by resize
    std::shared_ptr<SwapChain> swapChain;
    std::shared_ptr<FrameBuffer> geometryFrameBuffer;
    std::shared_ptr<FrameBuffer> UIFrameBuffer;
    VkExtent2D swapChainExtent;

    const std::vector<uint32_t>
        geometryRenderTargets =
        {RENDER_TARGET_DIFFUSE, RENDER_TARGET_POSITION, RENDER_TARGET_NORMAL, RENDER_TARGET_DEPTH};
    const std::vector<uint32_t>
        UIRenderTargets =
        {RENDER_TARGET_DIFFUSE, RENDER_TARGET_POSITION, RENDER_TARGET_NORMAL, RENDER_TARGET_DEPTH,
         RENDER_TARGET_AMBIENT};

    std::shared_ptr<RenderPass> geometryRenderPass;
    std::shared_ptr<RenderPass> UIRenderPass;

    uint32_t geometrySubpass;
    uint32_t selectorSubpass;
    uint32_t uiSubpass;

    std::vector<std::shared_ptr<Image>> renderTargets;
    std::vector<std::shared_ptr<ImageView>> renderTargetViews;

    std::shared_ptr<Image> depthImage;
    std::shared_ptr<ImageView> depthImageView;

    std::shared_ptr<FramebufferSelector> objectBuffer;
    std::shared_ptr<Image> objectBufferImage;
    std::shared_ptr<ImageView> objectBufferImageView;

    std::shared_ptr<ShadingPipeline> viewportSelector;
    std::shared_ptr<SSAOPipeline> ssaoPipeline;

    std::shared_ptr<Semaphore> ssaoSemaphore;
    std::shared_ptr<Semaphore> uiSemaphore;

    std::vector<std::function<void()>> frameFunctions{};

    // input objects
    bool mouseCaptured = false;
    const float mouseSpeed = 0.001f;

    uint32_t activePresentTarget = 0;

    // creation procedures
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPools();
    void createSwapChain();
    void createRenderPass();
    void createFrameBuffer();
    void createSceneObjects();
    void createPostProcessingPipelines();
    void createDefaultMaterialLayout();
    void createUIObjects();

    void createRenderObjects();
    void recreateSwapChain(bool newImageCount);

    // other procedures
    void processMouseInputs();

    // callbacks
    static void keyCallback(GLFWwindow *glfwWindow, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow *glfwWindow, int button, int action, int mods);

    void drawFrame_();

  public:
    /// Creates new RenderManager instance
    /// \param instance Vulkan instance to be used for rendering
    /// \param window Window to present to
    /// \param textureQuality Texture quality settings
    /// \param renderQuality Render quality settings
    /// \param cameraModel Camera model to use for active camera
    /// \return Handle to the new RenderManager instance
    static std::shared_ptr<RenderManager> create(const std::shared_ptr<Instance> &instance,
                                                 const std::shared_ptr<Window> &window,
                                                 const TextureQualitySettings &textureQuality,
                                                 const RenderQualityOptions &renderQuality,
                                                 const CameraModel &cameraModel);

    /// Resize the swapChain to fit the new dimensions of the window
    /// \param newImageCount Number of new swapChain images to use for buffering
    void resizeSwapChain(uint32_t newImageCount = 0);

    /// Update all data needed to render the scene
    void updateRenderData();
    /// Draw the frame
    void drawFrame();
    /// Process mouse and keyboard inputs
    void processInputs();

    /// Invalidate current frame and draw a new one
    static void invalidateFrame();
    /// Wait for all actions to finish execution
    void waitForExecution();
    /// Register a function to be called next frame
    void registerFunctionNextFrame(const std::function<void()> &function);
    /// Call all functions that have been registered
    void callFunctions();

    /// Load a mesh from a given file
    /// \param filename The filename of the mesh file
    /// \param normalizePos If the mesh position should be normalized
    void loadMesh(const std::string &filename, bool normalizePos = false);

    std::shared_ptr<Scene> getScene() { return scene; }

    std::shared_ptr<SceneWriter> getSceneWriter() { return sceneWriter; }

    std::shared_ptr<MaterialLibrary> getMaterialLibrary() { return materialLibrary; }

    std::shared_ptr<TextureLibrary> getTextureLibrary() { return textureLibrary; }

    std::shared_ptr<MeshLibrary> getMeshLibrary() { return meshLibrary; }

    std::shared_ptr<UIRenderer> getUIRenderer() { return uiRenderer; }

    std::shared_ptr<Device> getDevice() { return device; }

    std::shared_ptr<DescriptorPoolManager> getDescriptorManager() { return poolManager; }

    std::shared_ptr<MasterMaterialTemplate> getDefaultMaterialTemplate() { return defaultMaterialTemplate; }

    VkExtent2D getExtend() { return swapChainExtent; }

    ~RenderManager();
};

#endif //CUBICAD_RENDERMANAGER_H
