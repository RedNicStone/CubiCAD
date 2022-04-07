//
// Created by nic on 09/01/2022.
//

#include "scene.h"


std::shared_ptr<Scene> Scene::create(const std::shared_ptr<RenderManager> &renderManager,
                                     const std::shared_ptr<DescriptorPoolManager> &pPoolManager,
                                     const std::shared_ptr<Queue> &pGraphicsQueue,
                                     const std::shared_ptr<CommandPool> &pTransferPool,
                                     const std::shared_ptr<CommandPool> &pComputePool,
                                     const std::shared_ptr<Camera> &pCamera) {
    auto scene = std::make_shared<Scene>();
    scene->device = pPoolManager->getDevice();

    scene->renderManager = renderManager;
    scene->transferCommandPool = pTransferPool;
    scene->computeCommandPool = pComputePool;

    std::vector<uint32_t>
        accessingQueues
        {pTransferPool->getQueue()->getQueueFamilyIndex(), pComputePool->getQueue()->getQueueFamilyIndex(),
         pGraphicsQueue->getQueueFamilyIndex()};
    Utils::remove(accessingQueues);

    scene->instanceDataBuffer =
        DynamicBuffer::create(scene->device,
                              accessingQueues,
                              VMA_MEMORY_USAGE_GPU_ONLY,
                              VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    scene->indirectCommandBuffer =
        DynamicBuffer::create(scene->device,
                              accessingQueues,
                              VMA_MEMORY_USAGE_GPU_ONLY,
                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
                                  | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
                                  | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    scene->vertexBuffer =
        DynamicBuffer::create(scene->device,
                              accessingQueues,
                              VMA_MEMORY_USAGE_GPU_ONLY,
                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    scene->indexBuffer =
        DynamicBuffer::create(scene->device,
                              accessingQueues,
                              VMA_MEMORY_USAGE_GPU_ONLY,
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    scene->descriptorPool = DescriptorPoolManager::create(scene->device);

    scene->sceneBindings = std::vector<VkDescriptorSetLayoutBinding>(2);

    scene->sceneBindings[0].binding = 0;
    scene->sceneBindings[0].pImmutableSamplers = nullptr;
    scene->sceneBindings[0].stageFlags = VK_SHADER_STAGE_ALL;
    scene->sceneBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    scene->sceneBindings[0].descriptorCount = 1;

    scene->sceneBindings[1].binding = 1;
    scene->sceneBindings[1].pImmutableSamplers = nullptr;
    scene->sceneBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    scene->sceneBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    scene->sceneBindings[1].descriptorCount = 1;

    scene->sceneInfoSetLayout = DescriptorSetLayout::create(scene->device, scene->sceneBindings);
    scene->sceneDescriptorSet = scene->descriptorPool->allocate(scene->sceneInfoSetLayout);

    scene->sceneInfoBuffer = UniformBuffer::create(scene->device, pTransferPool->getQueue(), sizeof(SceneData));
    scene->sceneDescriptorSet->updateUniformBuffer(scene->sceneInfoBuffer, 0);

    scene->sceneDescriptorSet->updateStorageBuffer(scene->instanceDataBuffer->getBuffer(), 1);

    scene->instanceBufferData = scene->instanceDataBuffer->map();
    scene->indirectCommandBufferData = scene->indirectCommandBuffer->map();

    auto *data = static_cast<SceneData *>(scene->sceneInfoBuffer->getDataHandle());
    data->view = glm::lookAt(glm::vec3(2, 2, 2), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));

    scene->camera = pCamera;

    std::vector<VkDescriptorSetLayoutBinding> cullBindings(4);
    cullBindings[0].binding = 0;
    cullBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cullBindings[0].descriptorCount = 1;
    cullBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    cullBindings[0].pImmutableSamplers = nullptr;

    cullBindings[1].binding = 1;
    cullBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    cullBindings[1].descriptorCount = 1;
    cullBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    cullBindings[1].pImmutableSamplers = nullptr;

    cullBindings[2].binding = 2;
    cullBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    cullBindings[2].descriptorCount = 1;
    cullBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    cullBindings[2].pImmutableSamplers = nullptr;

    cullBindings[3].binding = 3;
    cullBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    cullBindings[3].descriptorCount = 1;
    cullBindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    cullBindings[3].pImmutableSamplers = nullptr;

    auto cullLayout = DescriptorSetLayout::create(scene->device, cullBindings);
    std::vector<std::shared_ptr<DescriptorSetLayout>> cullLayouts = {scene->sceneInfoSetLayout, cullLayout};

    auto pipelineLayout = PipelineLayout::create(scene->device, cullLayouts);
    scene->cullShader =
        ComputeShader::create(scene->device, "main", "resources/shaders/compiled/culling_indirect.comp.spv");
    scene->cullPipeline = ComputePipeline::create(scene->device, pipelineLayout, scene->cullShader);

    scene->cullInfoBuffer = UniformBuffer::create(scene->device, pTransferPool->getQueue(), sizeof(CullInfo));

    accessingQueues = {pComputePool->getQueue()->getQueueFamilyIndex(), pGraphicsQueue->getQueueFamilyIndex()};
    Utils::remove(accessingQueues);
    scene->instanceIndexBuffer =
        DynamicBuffer::create(scene->device,
                              accessingQueues,
                              VMA_MEMORY_USAGE_GPU_ONLY,
                              VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    scene->cullDescriptorSet = scene->descriptorPool->allocate(cullLayout);
    scene->cullDescriptorSet->updateUniformBuffer(scene->cullInfoBuffer, 0);
    scene->cullDescriptorSet->updateStorageBuffer(scene->instanceDataBuffer->getBuffer(), 1);
    scene->cullDescriptorSet->updateStorageBuffer(scene->indirectCommandBuffer->getBuffer(), 2);
    scene->cullDescriptorSet->updateStorageBuffer(scene->instanceIndexBuffer->getBuffer(), 3);

    scene->cullSemaphore = Semaphore::create(scene->device);

    return scene;
}

void Scene::setCamera(const std::shared_ptr<Camera> &pCamera) {
    camera = pCamera;
}

void Scene::updateUBO() {
    auto *data = static_cast<SceneData *>(sceneInfoBuffer->getDataHandle());

    camera->update();
    data->view = camera->getView();
    data->proj = camera->getProj();
    data->selectedID = selectedID;
    data->hoveredID = hoveredID;
}

void Scene::transferRenderData() {
    auto currentTime = std::chrono::high_resolution_clock::now();

    auto *data = static_cast<SceneData *>(sceneInfoBuffer->getDataHandle());
    data->frameTime = static_cast<glm::uint32>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(lastFrameTime - currentTime).count());
    data->nFrame++;

    lastFrameTime = currentTime;

    indirectDrawCalls.clear();
    std::vector<InstanceData> instanceData;
    std::vector<VkDrawIndexedIndirectCommand> indirectDrawData;

    glm::vec4 max{};
    glm::vec4 min{};

    /*for (const auto &instance: instances) {
        if (instance->getID() == 136) {
            auto
                vs_transform =
                static_cast<SceneData *>(sceneInfoBuffer->getDataHandle())->view
                    * static_cast<SceneData *>(sceneInfoBuffer->getDataHandle())->proj
                    * instance->getInstanceData().model;
            const auto &aabb = instance->getMesh()->getBoundingBox();

            glm::vec4 corners[8] = {{aabb.min.x, aabb.min.y, aabb.min.z, 1.0}, // x y z
                                    {aabb.max.x, aabb.min.y, aabb.min.z, 1.0}, // X y z
                                    {aabb.min.x, aabb.max.y, aabb.min.z, 1.0}, // x Y z
                                    {aabb.max.x, aabb.max.y, aabb.min.z, 1.0}, // X Y z

                                    {aabb.min.x, aabb.min.y, aabb.max.z, 1.0}, // x y Z
                                    {aabb.max.x, aabb.min.y, aabb.max.z, 1.0}, // X y Z
                                    {aabb.min.x, aabb.max.y, aabb.max.z, 1.0}, // x Y Z
                                    {aabb.max.x, aabb.max.y, aabb.max.z, 1.0}, // X Y Z
            };

            bool inside = false;

            for (const auto &corner_idx: corners) {
                // Transform vertex
                glm::vec4 corner = vs_transform * corner_idx;
                // Check vertex against clip space bounds
                inside = inside ||
                    //|| (abs(corner.x) < corner.w &&
                    //abs(corner.y) < corner.w &&
                    (0.0f < corner.z && corner.z < corner.w);

                corner /= corner.w;

                min = glm::min(min, corner);
                max = glm::max(max, corner);
            }
        }
    }*/

    auto
        vs_transform =
        static_cast<SceneData *>(sceneInfoBuffer->getDataHandle())->view
            * static_cast<SceneData *>(sceneInfoBuffer->getDataHandle())->proj
            * glm::mat4(1.0f);

    max = vs_transform * glm::vec4(1, 1, 1, 1.0);
    max /= max.w;
    min = max;

    std::cout
        << min.x
        << ", "
        << min.y
        << ", "
        << min.z
        << ", "
        << min.w
        << " | "
        << max.x
        << ", "
        << max.y
        << ", "
        << max.z
        << ", "
        << max.w
        << std::endl;

    auto instanceDataTempBuffer = instanceDataBuffer->getBuffer();
    auto indirectCommandTempBuffer = indirectCommandBuffer->getBuffer();
    auto instanceIndexTempBuffer = instanceIndexBuffer->getBuffer();

    std::unordered_map<std::shared_ptr<Meshlet>, std::vector<std::shared_ptr<MeshInstance>>> meshletDraws{};
    for (const auto &instance: instances) {
        for (const auto &meshlet: instance->getMesh()->getMeshlets()) {
            meshletDraws[meshlet].push_back(instance);
        }
    }
    std::unordered_map<std::shared_ptr<Material>,
                       std::vector<std::pair<std::shared_ptr<Meshlet>, std::vector<std::shared_ptr<MeshInstance>>>>>
        materialDraws{};
    for (const auto &meshletDraw: meshletDraws) {
        materialDraws[meshletDraw.first->material].push_back(meshletDraw);
    }
    std::unordered_map<std::shared_ptr<MasterMaterial>,
                       std::vector<std::pair<std::shared_ptr<Material>,
                                             std::vector<std::pair<std::shared_ptr<Meshlet>,
                                                                   std::vector<std::shared_ptr<MeshInstance>>>>>>>
        masterMaterialDraws{};
    for (const auto &materialDraw: materialDraws) {
        masterMaterialDraws[materialDraw.first->getMasterMaterial()].push_back(materialDraw);
    }
    for (const auto &drawCall: masterMaterialDraws) {
        for (const auto &materialCall: drawCall.second) {
            IndirectDrawCall indirectDrawCall{};
            indirectDrawCall.material = materialCall.first;
            indirectDrawCall.drawCallOffset = static_cast<uint32_t>(indirectDrawData.size());

            for (const auto &meshletCall: materialCall.second) {
                VkDrawIndexedIndirectCommand drawCommand = meshletCall.second.front()->getMesh()->getDrawCommand();
                drawCommand.firstInstance = static_cast<uint32_t>(instanceData.size());
                drawCommand.instanceCount = static_cast<uint32_t>(0);//meshletCall.second.size());

                for (const auto &instanceCall: meshletCall.second) {
                    instanceData.push_back(instanceCall->getInstanceData());
                    instanceData.back().batchID = static_cast<uint32_t>(indirectDrawData.size());
                }

                indirectDrawData.push_back(drawCommand);
            }

            indirectDrawCall.drawCallLength =
                static_cast<uint32_t>(indirectDrawData.size()) - indirectDrawCall.drawCallOffset;
            indirectDrawCalls.push_back(indirectDrawCall);
        }
    }

    if (instanceDataTempBuffer != instanceDataBuffer->getBuffer(instanceData.size() * sizeof(InstanceData))
        || indirectCommandTempBuffer
            != indirectCommandBuffer->getBuffer(indirectDrawData.size() * sizeof(VkDrawIndexedIndirectCommand))
        || instanceIndexTempBuffer != instanceIndexBuffer->getBuffer(instanceData.size())) {
        renderManager->registerFunctionNextFrame([&]() {
          renderManager->waitForExecution();

          cullDescriptorSet->updateStorageBuffer(instanceDataBuffer->getBuffer(), 1);
          cullDescriptorSet->updateStorageBuffer(indirectCommandBuffer->getBuffer(), 2);
          cullDescriptorSet->updateStorageBuffer(instanceIndexBuffer->getBuffer(), 3);

          sceneDescriptorSet->updateStorageBuffer(instanceDataBuffer->getBuffer(), 1);
        });
    }

    memcpy(*instanceBufferData, instanceData.data(), instanceData.size() * sizeof(InstanceData));
    memcpy(*indirectCommandBufferData,
           indirectDrawData.data(),
           indirectDrawData.size() * sizeof(VkDrawIndexedIndirectCommand));

    cullObjects(static_cast<uint32_t>(instanceData.size()));
}

void Scene::submitInstance(const std::shared_ptr<MeshInstance> &meshInstance) {
    if (meshInstance->getID() == 0)
        meshInstance->setID(static_cast<uint32_t>(instances.size() + 1));
    instances.push_back(meshInstance);
}

void Scene::clear() {
    instances.clear();
}

void Scene::collectRenderBuffers() {
    if (instances.empty())
        return;

    std::vector<Vertex> vertexData;
    std::vector<uint32_t> indexData;

    std::unordered_set<std::shared_ptr<Mesh>> meshes;
    for (const auto &instance: instances) {
        meshes.insert(instance->getMesh());
    }
    for (const auto &mesh: meshes) {
        mesh->setOffsets(static_cast<uint32_t>(indexData.size()), static_cast<uint32_t>(vertexData.size()));
        vertexData.insert(vertexData.end(), mesh->getVertexData().begin(), mesh->getVertexData().end());
        auto &meshlets = mesh->getMeshlets();
        for (const auto &meshlet: meshlets) {
            indexData.insert(indexData.end(), meshlet->indexData.begin(), meshlet->indexData.end());
        }
    }

    if (!vertexData.empty())
        vertexBuffer->getBuffer(sizeof(Vertex) * vertexData.size())
            ->transferDataStaged(vertexData.data(), transferCommandPool, sizeof(Vertex) * vertexData.size());

    if (!indexData.empty())
        indexBuffer->getBuffer(sizeof(uint32_t) * indexData.size())
            ->transferDataStaged(indexData.data(), transferCommandPool, sizeof(uint32_t) * indexData.size());
}

void Scene::bakeMaterials(bool enableDepthStencil) {
    std::unordered_set<std::shared_ptr<MasterMaterial>> masterMaterials{};
    for (const auto &instance: instances) {
        for (const auto &meshlet: instance->getMesh()->getMeshlets()) {
            masterMaterials.insert(meshlet->material->getMasterMaterial());
        }
    }

    for (const auto &masterMaterial: masterMaterials) {
        masterMaterial->updateDescriptorSetLayouts(sceneInfoSetLayout, enableDepthStencil);
    }
}

void Scene::bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer) {
    transferRenderData();

    std::vector<std::shared_ptr<Buffer>> vertexBuffers = {vertexBuffer->getBuffer(), instanceIndexBuffer->getBuffer()};
    graphicsCommandBuffer->bindVertexBuffers(vertexBuffers, 0);
    graphicsCommandBuffer->bindIndexBuffer(indexBuffer->getBuffer(), VK_INDEX_TYPE_UINT32);

    std::shared_ptr<MasterMaterial> previousMasterMaterial = nullptr;
    for (const auto &drawCall: indirectDrawCalls) {
        std::shared_ptr<MasterMaterial> currentMasterMaterial = drawCall.material->getMasterMaterial();
        if (currentMasterMaterial != previousMasterMaterial) {
            previousMasterMaterial = currentMasterMaterial;
            graphicsCommandBuffer->bindPipeline(currentMasterMaterial->getPipeline());
            std::vector<std::shared_ptr<DescriptorSet>>
                descriptorSets =
                {sceneDescriptorSet, currentMasterMaterial->getDescriptorSet(), drawCall.material->getDescriptorSet()};
            graphicsCommandBuffer->bindDescriptorSets(descriptorSets, currentMasterMaterial->getPipeline());
        } else {
            std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{drawCall.material->getDescriptorSet()};
            graphicsCommandBuffer->bindDescriptorSets(descriptorSets, currentMasterMaterial->getPipeline(), 2);
        }

        graphicsCommandBuffer->drawIndexedIndirect(indirectCommandBuffer->getBuffer(),
                                                   drawCall.drawCallLength,
                                                   drawCall.drawCallOffset * sizeof(VkDrawIndexedIndirectCommand),
                                                   sizeof(VkDrawIndexedIndirectCommand));
    }
}

Scene::~Scene() {
    instanceDataBuffer->unmap();
    indirectCommandBuffer->unmap();
}

void Scene::cullObjects(uint32_t meshCount) {
    auto computeBuffer = CommandBuffer::create(computeCommandPool);
    computeBuffer->beginCommandBuffer();

    computeBuffer->bindPipeline(cullPipeline);

    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets = {sceneDescriptorSet, cullDescriptorSet};
    computeBuffer->bindDescriptorSets(descriptorSets, cullPipeline);

    ((CullInfo *) cullInfoBuffer->getDataHandle())->cullCount = meshCount;
    computeBuffer->dispatch(static_cast<uint32_t>(instances.size() - 1) / 16 + 1, 1, 1);

    computeBuffer->endCommandBuffer();

    std::vector<std::shared_ptr<Semaphore>> signalSemaphores = {cullSemaphore};
    std::vector<std::shared_ptr<Semaphore>> waitSemaphores = {};
    computeBuffer->submitToQueue(signalSemaphores, {}, waitSemaphores, computeCommandPool->getQueue(), nullptr);
}
