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

#include "objectinstance.h"
#include "descriptorpoolmanager.h"
#include "dynamicbuffer.h"
#include "camera.h"


struct SceneData {
    glm::mat4 view{};  // view matrix
    glm::mat4 proj{};  // projection matrix

    glm::uint nFrame{};  // frame ID
    glm::uint frameTime{};  // frame time in ns
    glm::uint selectedID{};
    glm::uint hoverID{};
};

class Scene {
  public:
    struct IndirectDrawCall {
        std::shared_ptr<Material> material;
        uint32_t drawCallOffset{};
        uint32_t drawCallLength{};
    };

  private:
    std::shared_ptr<Camera> camera;

    std::shared_ptr<Device> device;

    std::shared_ptr<CommandPool> transferCommandPool;

    std::vector<std::shared_ptr<MeshInstance>> instances{};

    std::vector<IndirectDrawCall> indirectDrawCalls{};
    std::shared_ptr<DynamicBuffer> instanceBuffer;  // the instance buffer
    std::shared_ptr<DynamicBuffer> indirectCommandBuffer;  // the indirect command buffer
    void** instanceBufferData{};
    void** indirectCommandBufferData{};

    std::shared_ptr<DynamicBuffer> vertexBuffer;
    std::shared_ptr<DynamicBuffer> indexBuffer;

    std::shared_ptr<UniformBuffer> sceneInfoBuffer;
    std::vector<VkDescriptorSetLayoutBinding> sceneBindings{};
    std::shared_ptr<DescriptorSetLayout> sceneInfoSetLayout;
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> lastFrameTime{};

    std::shared_ptr<DescriptorPoolManager> descriptorPool;
    std::shared_ptr<DescriptorSet> sceneDescriptorSet;

    void transferRenderData();

  public:
    static std::shared_ptr<Scene> create(const std::shared_ptr<Device>& pDevice,
                                  const std::shared_ptr<Queue>& pTransferQueue,
                                  const std::shared_ptr<Queue>& pGraphicsQueue,
                                  const std::shared_ptr<Camera>& pCamera);

    void setCamera(const std::shared_ptr<Camera>& pCamera);
    void updateUBO();

    void submitInstance(const std::shared_ptr<MeshInstance>& meshInstance);

    void collectRenderBuffers();
    void bakeMaterials(bool enableDepthStencil = false);

    void bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer);

    std::shared_ptr<Camera> getCamera() { return camera; }

    ~Scene();
};

#endif //CUBICAD_SCENE_H
