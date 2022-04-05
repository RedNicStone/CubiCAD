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


struct SceneData {
    glm::mat4 view{};       // view matrix
    glm::mat4 proj{};       // projection matrix

    glm::uint nFrame{};     // frame ID
    glm::uint frameTime{};  // frame time in ns
    glm::uint selectedID{}; // ID of the currently selected object
    glm::uint hoveredID{};  // ID of the currently hovered object
};

struct CullInfo {
    glm::uint cullCount{};    // number of objects to test for culling
};

class Scene {
  public:
    struct IndirectDrawCall {
        std::shared_ptr<Material> material; // material drawn with
        uint32_t drawCallOffset{};          // offset of first drawCallCmd in array
        uint32_t drawCallLength{};          // number of drawCallCmd to draw
    };

  private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Device> device;

    std::shared_ptr<CommandPool> transferCommandPool;
    std::shared_ptr<CommandPool> computeCommandPool;

    std::vector<std::shared_ptr<MeshInstance>> instances{};

    std::vector<IndirectDrawCall> indirectDrawCalls{};
    std::shared_ptr<DynamicBuffer> instanceDataBuffer;  // the instance buffer
    std::shared_ptr<DynamicBuffer> instanceIndexBuffer;
    std::shared_ptr<DynamicBuffer> indirectCommandBuffer;  // the indirect command buffer
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

    uint32_t selectedID{};
    uint32_t hoveredID{};

    void transferRenderData();

    void cullObjects();

  public:
    /// Constructor for handle to scene
    /// \param pDevice Device to operate on
    /// \param pTransferQueue Transfer queue to use for data transfers
    /// \param pGraphicsQueue Graphics queue to use for
    /// \param pCamera
    /// \return
    static std::shared_ptr<Scene> create(const std::shared_ptr<DescriptorPoolManager> &pPoolManager,
                                         const std::shared_ptr<Queue> &pGraphicsQueue,
                                         const std::shared_ptr<CommandPool> &pTransferPool,
                                         const std::shared_ptr<CommandPool> &pComputePool,
                                         const std::shared_ptr<Camera> &pCamera);

    void setCamera(const std::shared_ptr<Camera> &pCamera);
    void updateUBO();

    void setSelected(uint32_t objectID) { selectedID = objectID; }

    void setHovered(uint32_t objectID) { hoveredID = objectID; }

    [[nodiscard]] uint32_t getSelected() const { return selectedID; }

    [[nodiscard]] uint32_t getHovered() const { return hoveredID; }

    void submitInstance(const std::shared_ptr<MeshInstance> &meshInstance);
    void clear();

    void collectRenderBuffers();
    void bakeMaterials(bool enableDepthStencil = false);

    void bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer);

    std::shared_ptr<Camera> getCamera() { return camera; }
    std::vector<std::shared_ptr<MeshInstance>> getInstances() { return instances; }
    std::shared_ptr<MeshInstance> getInstanceByID(uint32_t objectID) { return instances[objectID - 1]; }
    std::shared_ptr<DescriptorSet> getDescriptorSet() { return sceneDescriptorSet; }

    std::shared_ptr<Semaphore> getWaitSemaphore() { return cullSemaphore; }

    ~Scene();
};

#endif //CUBICAD_SCENE_H
