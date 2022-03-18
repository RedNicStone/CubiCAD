//
// Created by nic on 18/03/2022.
//

#include "ssaopipeline.h"


std::shared_ptr<SSAOPipeline> SSAOPipeline::create(const std::shared_ptr<Device> &pDevice,
                                                   const std::shared_ptr<DescriptorPoolManager> &poolManager,
                                                   const std::shared_ptr<Queue> &transferQueue,
                                                   const std::shared_ptr<ImageView>
                                                   &resultImageView,
                                                   const std::shared_ptr<ImageView>
                                                   &normalImageView,
                                                   const std::shared_ptr<ImageView>
                                                   &posImageView,
                                                   VkExtent2D imageExtend,
                                                   uint32_t sampleCount) {
    auto ssaoPipeline = std::make_shared<SSAOPipeline>();
    ssaoPipeline->imageExtend = imageExtend;

    std::vector<VkDescriptorSetLayoutBinding> sampleDescriptorSetLayoutBinding(5);
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

    std::vector<std::shared_ptr<DescriptorSetLayout>> sampleDescriptorSetLayout =
        { DescriptorSetLayout::create(pDevice, sampleDescriptorSetLayoutBinding) };


    auto obscurancePipelineLayout = PipelineLayout::create(pDevice, sampleDescriptorSetLayout);

    auto obscuranceShader = ComputeShader::create(pDevice, "main", "resources/shaders/compiled/alchemy_ssao.comp.spv");

    ssaoPipeline->obscurancePipeline = ComputePipeline::create(pDevice, obscurancePipelineLayout, obscuranceShader);

    std::random_device rd;
    std::default_random_engine uniform(rd());
    auto generatorU = [&](){
        return std::uniform_real_distribution<float>(0.0f, 0.125f)(uniform);};
    auto generatorS = [&](){
        return std::uniform_real_distribution<float>(-0.125f, 0.125f)(uniform);};

    PassInfo passInfo{};
    passInfo.imageSize = { imageExtend.width, imageExtend.height };
    passInfo.samples.resize(sampleCount);
    for (uint32_t i = 0; i < sampleCount; i++)
        passInfo.samples[i] = glm::vec4(std::clamp(0.25f + generatorU(), 0.0f, 1.0f),
                                std::clamp(0.50f + generatorS(), 0.0f, 1.0f),
                                std::clamp(0.75f + generatorS(), 0.0f, 1.0f),
                                std::clamp(1.00f - generatorU(), 0.0f, 1.0f));

    ssaoPipeline->sampleUniform = UniformBuffer::create(pDevice, transferQueue, sizeof(PassInfo));
    memcpy(ssaoPipeline->sampleUniform->getDataHandle(), &passInfo.imageSize, sizeof(glm::uvec2));
    memcpy((char*)ssaoPipeline->sampleUniform->getDataHandle() + offsetof(PassInfo, samples),
           passInfo.samples.data(),sizeof(glm::vec4) * passInfo.samples.size());

    ssaoPipeline->imageSampler = Sampler::create(pDevice, 0.0f);

    ssaoPipeline->sampleDescriptorSet = poolManager->allocate(sampleDescriptorSetLayout[0]);
    ssaoPipeline->sampleDescriptorSet->updateUniformBuffer(ssaoPipeline->sampleUniform, 0);
    ssaoPipeline->sampleDescriptorSet->updateImageSampler(ssaoPipeline->imageSampler, 1);
    ssaoPipeline->sampleDescriptorSet->updateImage(resultImageView, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2);
    ssaoPipeline->sampleDescriptorSet->updateImage(normalImageView, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 3);
    ssaoPipeline->sampleDescriptorSet->updateImage(posImageView, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4);

    return ssaoPipeline;
}

void SSAOPipeline::bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer) {
    graphicsCommandBuffer->bindPipeline(obscurancePipeline);

    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets = { sampleDescriptorSet };
    graphicsCommandBuffer->bindDescriptorSets(descriptorSets, obscurancePipeline);

    graphicsCommandBuffer->dispatch((imageExtend.width - 1) / 16 + 1, (imageExtend.height - 1) / 16 + 1, 1);
}
