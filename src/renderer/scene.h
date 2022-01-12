//
// Created by nic on 09/01/2022.
//

#pragma once

#ifndef CUBICAD_SCENE_H
#define CUBICAD_SCENE_H

#include <unordered_set>


#include "objectinstance.h"
#include "descriptorpoolmanager.h"


struct SceneData {
    alignas(64) glm::mat4 view;  // view matrix
    alignas(64) glm::mat4 proj;  // projection matrix

    alignas(4)  glm::uint nFrame;  // frame ID
    alignas(4)  glm::uint frameTime;  // frame time in ns
};

class Scene {
  public:
    struct IndirectDrawCall {
        std::shared_ptr<Material> material;
        uint32_t drawCallOffset;
        uint32_t drawCallLength;
    };

  private:
    std::shared_ptr<Device> device;

    std::shared_ptr<CommandPool> transferCommandPool;
    std::shared_ptr<CommandPool> graphicsCommandPool;
    std::shared_ptr<CommandBuffer> graphicsCommandBuffer;

    std::vector<std::shared_ptr<MeshInstance>> instances;

    std::vector<IndirectDrawCall> indirectDrawCalls;
    std::shared_ptr<Buffer> instanceBuffer;  // the instance buffer
    std::shared_ptr<Buffer> indirectCommandBuffer;  // the indirect command buffer
    void* instanceBufferData;
    void* indirectCommandBufferData;

    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;

    std::shared_ptr<UniformBuffer<SceneData>> sceneInfoBuffer;
    std::vector<VkDescriptorSetLayoutBinding> sceneBindings;
    std::shared_ptr<DescriptorSetLayout> sceneInfoSetLayout;
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> lastFrameTime;

    std::shared_ptr<DescriptorPoolManager> descriptorPool;
    std::shared_ptr<DescriptorSet> sceneDescriptorSet;
    std::shared_ptr<PipelineLayout> pipelineLayout;

    void transferRenderData();

  public:
    static std::shared_ptr<Scene> create(const std::shared_ptr<Device>& pDevice,
                                  const std::shared_ptr<Queue>& pTransferQueue,
                                  const std::shared_ptr<Queue>& pGraphicsQueue,
                                  size_t reservedInstances = 65536,
                                  size_t reservedIndirectCommands = 256);

    void submitInstance(const std::shared_ptr<MeshInstance>& meshInstance);

    void collectRenderBuffers();
    void bakeMaterials();

    std::shared_ptr<CommandBuffer> bakeGraphicsBuffer();

    ~Scene();
};

#endif //CUBICAD_SCENE_H
