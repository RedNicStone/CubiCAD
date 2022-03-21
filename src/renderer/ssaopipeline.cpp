//
// Created by nic on 18/03/2022.
//

#include "ssaopipeline.h"


std::shared_ptr<SSAOPipeline> SSAOPipeline::create(const std::shared_ptr<Device> &pDevice,
                                                   const std::shared_ptr<DescriptorPoolManager> &poolManager,
                                                   const std::shared_ptr<CommandPool> &transferPool,
                                                   const std::shared_ptr<CommandPool> &computePool,
                                                   const std::shared_ptr<ImageView>
                                                   &resultImageView,
                                                   const std::shared_ptr<ImageView>
                                                   &normalImageView,
                                                   const std::shared_ptr<ImageView>
                                                   &posImageView,
                                                   const std::shared_ptr<DescriptorSet>& sceneDescriptor,
                                                   float fov,
                                                   VkExtent2D imageExtend,
                                                   uint32_t sampleCount) {
    auto ssaoPipeline = std::make_shared<SSAOPipeline>();
    ssaoPipeline->imageExtend = imageExtend;
    ssaoPipeline->sceneDescriptorSet = sceneDescriptor;

    std::vector<VkDescriptorSetLayoutBinding> sampleDescriptorSetLayoutBinding(6);
    sampleDescriptorSetLayoutBinding[0].binding = 0;
    sampleDescriptorSetLayoutBinding[0].descriptorCount = 1;
    sampleDescriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sampleDescriptorSetLayoutBinding[0].pImmutableSamplers = VK_NULL_HANDLE;
    sampleDescriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    sampleDescriptorSetLayoutBinding[1].binding = 1;
    sampleDescriptorSetLayoutBinding[1].descriptorCount = 1;
    sampleDescriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    sampleDescriptorSetLayoutBinding[1].pImmutableSamplers = VK_NULL_HANDLE;
    sampleDescriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    sampleDescriptorSetLayoutBinding[2].binding = 2;
    sampleDescriptorSetLayoutBinding[2].descriptorCount = 1;
    sampleDescriptorSetLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    sampleDescriptorSetLayoutBinding[2].pImmutableSamplers = VK_NULL_HANDLE;
    sampleDescriptorSetLayoutBinding[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    sampleDescriptorSetLayoutBinding[3].binding = 3;
    sampleDescriptorSetLayoutBinding[3].descriptorCount = 1;
    sampleDescriptorSetLayoutBinding[3].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    sampleDescriptorSetLayoutBinding[3].pImmutableSamplers = VK_NULL_HANDLE;
    sampleDescriptorSetLayoutBinding[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    sampleDescriptorSetLayoutBinding[4].binding = 4;
    sampleDescriptorSetLayoutBinding[4].descriptorCount = 1;
    sampleDescriptorSetLayoutBinding[4].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    sampleDescriptorSetLayoutBinding[4].pImmutableSamplers = VK_NULL_HANDLE;
    sampleDescriptorSetLayoutBinding[4].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    sampleDescriptorSetLayoutBinding[5].binding = 5;
    sampleDescriptorSetLayoutBinding[5].descriptorCount = 1;
    sampleDescriptorSetLayoutBinding[5].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    sampleDescriptorSetLayoutBinding[5].pImmutableSamplers = VK_NULL_HANDLE;
    sampleDescriptorSetLayoutBinding[5].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    std::vector<std::shared_ptr<DescriptorSetLayout>> sampleDescriptorSetLayout =
        { sceneDescriptor->getLayout(),
          DescriptorSetLayout::create(pDevice, sampleDescriptorSetLayoutBinding) };

    auto pipelineLayout = PipelineLayout::create(pDevice, sampleDescriptorSetLayout);

    auto obscuranceShader = ComputeShader::create(pDevice, "main", "resources/shaders/compiled/alchemy_ssao.comp.spv");
    auto blurShader = ComputeShader::create(pDevice, "main", "resources/shaders/compiled/blur.comp.spv");

    ssaoPipeline->obscurancePipeline = ComputePipeline::create(pDevice, pipelineLayout, obscuranceShader);
    ssaoPipeline->blurPipeline = ComputePipeline::create(pDevice, pipelineLayout, blurShader);

    const auto DEG_TO_RAD = static_cast<float>(M_PI / 180);

    std::random_device rd;
    std::default_random_engine uniform(rd());
    auto generator = [&](){
        return std::uniform_real_distribution<float>(0.0f, 1.0f)(uniform);};

    struct sample {
        alignas(16) glm::vec3 p;
    };

    auto samples = std::vector<sample>(sampleCount);
    for (uint32_t i = 0; i < sampleCount; i++) {
        samples[i].p = glm::normalize(glm::vec3(generator() * 2.0f - 1.0f,
                                                generator() * 2.0f - 1.0f,
                                                generator()));
        float scale = (float) sampleCount / 64.0f;
        scale   = Utils::lerp(0.1f, 1.0f, scale * scale);
        samples[i].p *= scale;
    }

    PassInfo passInfo{};
    passInfo.imageSize = { imageExtend.width, imageExtend.height };
    passInfo.tanFOV = std::tan(fov * DEG_TO_RAD);

    ssaoPipeline->sampleUniform = UniformBuffer::create(pDevice, transferPool->getQueue(),
                                                        sizeof(PassInfo) + samples.size() * sizeof(sample));
    memcpy(ssaoPipeline->sampleUniform->getDataHandle(), &passInfo, sizeof(passInfo));
    memcpy((char*)ssaoPipeline->sampleUniform->getDataHandle() + sizeof(passInfo),
           samples.data(), samples.size() * sizeof(sample));

    ssaoPipeline->imageSampler = Sampler::create(pDevice, 0.0f);

    std::vector<glm::vec2> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec2 noise(
                    generator() * 2.0f - 1.0f,
                    generator() * 2.0f - 1.0f);
        ssaoNoise.push_back(noise);
    }

    std::vector<uint32_t> accessingQueues = {computePool->getQueue()->getQueueFamilyIndex()};
    ssaoPipeline->noiseImage = Image::create(pDevice, {4, 4}, 1, VK_FORMAT_R32G32_SFLOAT,
                                                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                                accessingQueues);
    ssaoPipeline->noiseImage->transferDataStaged(ssaoNoise.data(), computePool, static_cast<VkDeviceSize>(
        static_cast<uint>(16 * 8)), 0, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
    ssaoPipeline->noiseImageView = ssaoPipeline->noiseImage->createImageView(VK_IMAGE_VIEW_TYPE_2D,
                                                                             {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

    ssaoPipeline->sampleDescriptorSet = poolManager->allocate(sampleDescriptorSetLayout[1]);
    ssaoPipeline->sampleDescriptorSet->updateUniformBuffer(ssaoPipeline->sampleUniform, 0);
    ssaoPipeline->sampleDescriptorSet->updateImageSampler(ssaoPipeline->imageSampler, 1);
    ssaoPipeline->sampleDescriptorSet->updateImage(resultImageView, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                                   VK_IMAGE_LAYOUT_GENERAL, 2);
    ssaoPipeline->sampleDescriptorSet->updateImage(normalImageView, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 3);
    ssaoPipeline->sampleDescriptorSet->updateImage(posImageView, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 4);
    ssaoPipeline->sampleDescriptorSet->updateImage(ssaoPipeline->noiseImageView, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 5);

    return ssaoPipeline;
}

void SSAOPipeline::bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer) {
    graphicsCommandBuffer->bindPipeline(obscurancePipeline);

    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets = { sceneDescriptorSet, sampleDescriptorSet };
    graphicsCommandBuffer->bindDescriptorSets(descriptorSets, obscurancePipeline);

    graphicsCommandBuffer->dispatch((imageExtend.width - 1) / 16 + 1, (imageExtend.height - 1) / 16 + 1, 1);

    graphicsCommandBuffer->bindPipeline(blurPipeline);

    graphicsCommandBuffer->bindDescriptorSets(descriptorSets, blurPipeline);

    graphicsCommandBuffer->dispatch((imageExtend.width - 1) / 16 + 1, (imageExtend.height - 1) / 16 + 1, 1);
}
