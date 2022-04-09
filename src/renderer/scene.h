//
// Created by nic on 09/01/2022.
//

#pragma once

#ifndef CUBICAD_SCENE_H
#define CUBICAD_SCENE_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <unordered_set>
#include <glm/gtc/matrix_transform.hpp>

#include "meshinstance.h"
#include "descriptorpoolmanager.h"
#include "dynamicbuffer.h"
#include "camera.h"
#include "vulkan/computepipeline.h"
#include "rendermanager.h"


class RenderManager;

/// Structure that is shared with the GPU which stores data regarding the current scene
struct SceneData {
    glm::mat4 view{};       //< view matrix
    glm::mat4 proj{};       //< projection matrix

    glm::uint nFrame{};     //< frame ID
    glm::uint frameTime{};  //< frame time in ns
    glm::uint selectedID{}; //< ID of the currently selected object
    glm::uint hoveredID{};  //< ID of the currently hovered object
};

/// Structure shared with the GPU side during culling
struct CullInfo {
    glm::uint cullCount{};    //< number of objects to test for culling
};

/// Scene class that stores all objects in a scene and manages rendering them
class Scene {
  public:
    /// Structure storing information regarding each draw call
    struct IndirectDrawCall {
        std::shared_ptr<Material> material; //< material drawn with
        uint32_t drawCallOffset{};          //< offset of first drawCallCmd in array
        uint32_t drawCallLength{};          //< number of drawCallCmd to draw
    };

  private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Device> device;

    std::shared_ptr<CommandPool> transferCommandPool;
    std::shared_ptr<CommandPool> computeCommandPool;

    std::vector<std::shared_ptr<MeshInstance>> instances{};

    std::vector<IndirectDrawCall> indirectDrawCalls{};
    std::shared_ptr<DynamicBuffer> instanceDataBuffer;
    std::shared_ptr<DynamicBuffer> instanceIndexBuffer;
    std::shared_ptr<DynamicBuffer> indirectCommandBuffer;
    void **instanceBufferData{};
    void **indirectCommandBufferData{};

    std::shared_ptr<DynamicBuffer> vertexBuffer;
    std::shared_ptr<DynamicBuffer> indexBuffer;

    std::shared_ptr<UniformBuffer> sceneInfoBuffer;
    std::vector<VkDescriptorSetLayoutBinding> sceneBindings{};
    std::shared_ptr<DescriptorSetLayout> sceneInfoSetLayout;
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> lastFrameTime{};

    std::shared_ptr<DescriptorPoolManager> descriptorPool;
    std::shared_ptr<DescriptorSet> sceneDescriptorSet;

    std::shared_ptr<UniformBuffer> cullInfoBuffer;
    std::shared_ptr<DescriptorSet> cullDescriptorSet;
    std::shared_ptr<Semaphore> cullSemaphore;
    std::shared_ptr<ComputeShader> cullShader;
    std::shared_ptr<ComputePipeline> cullPipeline;

    std::shared_ptr<RenderManager> renderManager;

    uint32_t selectedID{};
    uint32_t hoveredID{};

    void transferRenderData();

    void cullObjects(uint32_t meshCount);

  public:
    /// Constructor creating a valid scene handle
    /// \param renderManager RenderManager instance to use for rendering
    /// \param pPoolManager PoolManager object for allocating descriptor sets
    /// \param pGraphicsQueue Graphics queue to use for all rendering commands
    /// \param pTransferPool Transfer pool for transferring data
    /// \param pComputePool Compute pool to run the cull shader
    /// \param pCamera Camera to use for rendering the scene
    /// \return Valid scene handle
    static std::shared_ptr<Scene> create(const std::shared_ptr<RenderManager> &renderManager,
                                         const std::shared_ptr<DescriptorPoolManager> &pPoolManager,
                                         const std::shared_ptr<Queue> &pGraphicsQueue,
                                         const std::shared_ptr<CommandPool> &pTransferPool,
                                         const std::shared_ptr<CommandPool> &pComputePool,
                                         const std::shared_ptr<Camera> &pCamera);

    /// Sets a new camera as active camera
    /// \param pCamera Handle to camera to be used
    void setCamera(const std::shared_ptr<Camera> &pCamera);
    /// Update scene data on the GPU side
    void updateUBO();

    /// Set what object is currently selected
    /// \param objectID ID of the object
    void setSelected(uint32_t objectID) { selectedID = objectID; }
    /// Set what objects is currently being hovered
    /// \param objectID ID of the object
    void setHovered(uint32_t objectID) { hoveredID = objectID; }

    [[nodiscard]] uint32_t getSelected() const { return selectedID; }

    [[nodiscard]] uint32_t getHovered() const { return hoveredID; }

    /// Submit a new instance to the scene
    /// \param meshInstance The handle to the instance
    void submitInstance(const std::shared_ptr<MeshInstance> &meshInstance);
    /// Clear the scene and remove all objects
    void clear();

    /// Collect buffers needed for rendering
    void collectRenderBuffers();
    /// Bake all currently used materials
    /// \param enableDepthStencil Enable depth stencil testing
    void bakeMaterials(bool enableDepthStencil = false);

    /// Bake all buffer required for rendering
    /// \param graphicsCommandBuffer Buffer to bake into
    void bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer);

    std::shared_ptr<Camera> getCamera() { return camera; }

    std::vector<std::shared_ptr<MeshInstance>> getInstances() { return instances; }

    std::shared_ptr<MeshInstance> getInstanceByID(uint32_t objectID) { return instances[objectID - 1]; }

    std::shared_ptr<DescriptorSet> getDescriptorSet() { return sceneDescriptorSet; }

    std::shared_ptr<Semaphore> getWaitSemaphore() { return cullSemaphore; }

    ~Scene();
};

#endif //CUBICAD_SCENE_H
