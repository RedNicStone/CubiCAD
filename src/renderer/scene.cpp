//
// Created by nic on 09/01/2022.
//

#include "scene.h"


std::shared_ptr<Scene> Scene::create(const std::shared_ptr<Device> &pDevice,
                                     const std::shared_ptr<Queue> &pTransferQueue,
                                     const std::shared_ptr<Queue> &pGraphicsQueue,
                                     size_t reservedInstances,
                                     size_t reservedIndirectCommands) {
    auto scene = std::make_shared<Scene>();
    scene->device = pDevice;

    scene->transferCommandPool = CommandPool::create(pDevice, pTransferQueue, 0);
    scene->graphicsCommandPool = CommandPool::create(pDevice, pGraphicsQueue, 0);

    scene->instanceBuffer = Buffer::create(pDevice, sizeof(InstanceData) * reservedInstances,
                                           VMA_MEMORY_USAGE_GPU_ONLY,
                                           VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           { pTransferQueue->getQueueFamilyIndex(), pGraphicsQueue->getQueueFamilyIndex() });
    scene->indirectCommandBuffer = Buffer::create(pDevice, sizeof(VkDrawIndexedIndirectCommand) * reservedIndirectCommands,
                                           VMA_MEMORY_USAGE_GPU_ONLY,
                                           VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           { pTransferQueue->getQueueFamilyIndex(), pGraphicsQueue->getQueueFamilyIndex() });

    scene->descriptorPool = DescriptorPoolManager::create(pDevice);

    scene->sceneBindings = {
        { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL_GRAPHICS, VK_NULL_HANDLE }
    };
    scene->sceneInfoSetLayout = DescriptorSetLayout::create(pDevice, scene->sceneBindings);
    scene->sceneDescriptorSet = scene->descriptorPool->allocate(scene->sceneInfoSetLayout);

    scene->sceneInfoBuffer = UniformBuffer<SceneData>::create(pDevice, pTransferQueue);

    scene->instanceBufferData = scene->instanceBuffer->map();
    scene->indirectCommandBufferData = scene->indirectCommandBuffer->map();

    scene->graphicsCommandBuffer = CommandBuffer::create(scene->graphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    return scene;
}

void Scene::transferRenderData() {
    auto currentTime = std::chrono::high_resolution_clock::now();

    SceneData* data = sceneInfoBuffer->getDataHandle();
    data->frameTime = static_cast<glm::uint32>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(lastFrameTime - currentTime).count());
    data->nFrame++;

    lastFrameTime = currentTime;

    indirectDrawCalls.clear();
    std::vector<InstanceData> instanceData;
    std::vector<VkDrawIndexedIndirectCommand> indirectDrawData;

    std::unordered_map<std::shared_ptr<Meshlet>,
                       std::vector<std::shared_ptr<MeshInstance>>> meshletDraws{};
    for (const auto& instance : instances) {
        for (const auto& meshlet : instance->getMesh()->getMeshlets()) {
            meshletDraws[meshlet].push_back(instance);
        }
    }
    std::unordered_map<std::shared_ptr<Material>,
                       std::vector<std::pair<std::shared_ptr<Meshlet>,
                                             std::vector<std::shared_ptr<MeshInstance>>>>> materialDraws{};
    for (const auto& meshletDraw : meshletDraws) {
        materialDraws[meshletDraw.first->material].push_back(meshletDraw);
    }
    std::unordered_map<std::shared_ptr<MasterMaterial>,
                       std::vector<std::pair<std::shared_ptr<Material>,
                                   std::vector<std::pair<std::shared_ptr<Meshlet>,
                                               std::vector<std::shared_ptr<MeshInstance>>>>>>> masterMaterialDraws{};
    for (const auto& materialDraw : materialDraws) {
        masterMaterialDraws[materialDraw.first->getMasterMaterial()].push_back(materialDraw);
    }
    for (const auto& drawCall : masterMaterialDraws) {
        for (const auto& materialCall : drawCall.second) {
            IndirectDrawCall indirectDrawCall{};
            indirectDrawCall.material = materialCall.first;
            indirectDrawCall.drawCallOffset = static_cast<uint32_t>(indirectDrawCalls.size());

            for (const auto& meshletCall : materialCall.second) {
                for (const auto& instanceCall : meshletCall.second) {
                    instanceData.push_back(instanceCall->getInstanceData());
                }

                VkDrawIndexedIndirectCommand drawCommand = meshletCall.second.front()->getMesh()->getDrawCommand();
                drawCommand.firstInstance = 0;
                drawCommand.instanceCount = static_cast<uint32_t>(drawCall.second.size());
                indirectDrawData.push_back(drawCommand);
            }

            indirectDrawCall.drawCallLength = static_cast<uint32_t>(indirectDrawCalls.size())
                - indirectDrawCall.drawCallOffset;
            indirectDrawCalls.push_back(indirectDrawCall);
        }
    }

    memcpy(instanceBufferData, instanceData.data(), instanceData.size() * sizeof(InstanceData));
    memcpy(indirectCommandBufferData, indirectDrawData.data(), indirectDrawData.size() * sizeof
    (VkDrawIndexedIndirectCommand));
}

void Scene::submitInstance(const std::shared_ptr<MeshInstance>& meshInstance) {
    instances.push_back(meshInstance);
}

void Scene::collectRenderBuffers() {
    std::vector<Vertex> vertexData;
    std::vector<uint32_t> indexData;

    std::unordered_set<std::shared_ptr<Mesh>> meshes;
    for (const auto& instance : instances) {
        meshes.insert(instance->getMesh());
    }
    for (const auto& mesh : meshes) {
        mesh->setOffsets(static_cast<uint32_t>(indexData.size()), static_cast<uint32_t>(vertexData.size()));
        auto& meshlets = mesh->getMeshlets();
        for (const auto& meshlet : meshlets) {
            vertexData.insert(vertexData.end(), meshlet->vertexData.begin(), meshlet->vertexData.end());
            indexData.insert(indexData.end(), meshlet->indexData.begin(), meshlet->indexData.end());
        }
    }

    vertexBuffer->transferDataStaged(vertexData.data(), transferCommandPool);
    indexBuffer->transferDataStaged(indexData.data(), transferCommandPool);
}

void Scene::bakeMaterials() {
    std::unordered_set<std::shared_ptr<MasterMaterial>> masterMaterials;
    for (const auto& instance : instances) {
        for (const auto& meshlet : instance->getMesh()->getMeshlets()) {
            masterMaterials.insert(meshlet->material->getMasterMaterial());
        }
    }

    for (const auto& masterMaterial : masterMaterials) {
        masterMaterial->updateDescriptorSetLayouts(sceneInfoSetLayout);
    }
}

std::shared_ptr<CommandBuffer> Scene::bakeGraphicsBuffer() {
    transferRenderData();

    graphicsCommandBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    std::vector<std::shared_ptr<DescriptorSet>> sceneDescriptorSets{ sceneDescriptorSet };

    std::vector<uint32_t> offsets{};
    graphicsCommandBuffer->bindDescriptorSets(sceneDescriptorSets, pipelineLayout, VK_PIPELINE_BIND_POINT_GRAPHICS, offsets);

    std::vector<std::shared_ptr<Buffer>> vertexBuffers = {vertexBuffer, instanceBuffer};
    graphicsCommandBuffer->bindVertexBuffers(vertexBuffers, 0);
    graphicsCommandBuffer->bindIndexBuffer(indexBuffer, VK_INDEX_TYPE_UINT32);

    std::shared_ptr<MasterMaterial> previousMasterMaterial{};
    for (const auto& drawCall : indirectDrawCalls) {
        std::shared_ptr<MasterMaterial> currentMasterMaterial = drawCall
            .material->getMasterMaterial();
        if (currentMasterMaterial != previousMasterMaterial) {
            previousMasterMaterial = currentMasterMaterial;
            graphicsCommandBuffer->bindPipeline(currentMasterMaterial->getPipeline());
            std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{ currentMasterMaterial->getDescriptorSet(),
                                                                                      drawCall.material->getDescriptorSet() };
            graphicsCommandBuffer->bindDescriptorSets(descriptorSets, currentMasterMaterial->getPipeline());
        } else {
            std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{ drawCall.material->getDescriptorSet() };
            graphicsCommandBuffer->bindDescriptorSets(descriptorSets, currentMasterMaterial->getPipeline());
        }

        graphicsCommandBuffer->drawIndexedIndirect(indirectCommandBuffer, drawCall.drawCallLength, drawCall
        .drawCallOffset);
    }

    graphicsCommandBuffer->endRenderPass();

    graphicsCommandBuffer->endCommandBuffer();
}

Scene::~Scene() {
    instanceBuffer->unmap();
    indirectCommandBuffer->unmap();
}
