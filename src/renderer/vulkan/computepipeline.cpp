//
// Created by nic on 08/03/2021.
//

#include "computepipeline.h"


std::shared_ptr<ComputePipeline> ComputePipeline::create(const std::shared_ptr<Device> &pDevice,
                                                         const std::shared_ptr<PipelineLayout> &pLayout,
                                                         const std::shared_ptr<ComputeShader> &shader,
                                                         const VkSpecializationInfo *shaderSpecialization) {
    auto computePipeline = std::make_shared<ComputePipeline>();
    computePipeline->device = pDevice;
    computePipeline->layout = pLayout;

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage = shader->pipelineStageInfo();
    pipelineInfo.stage.pSpecializationInfo = shaderSpecialization;
    pipelineInfo.layout = computePipeline->layout->getHandle();
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateComputePipelines(computePipeline->device->getHandle(),
                                 VK_NULL_HANDLE,
                                 1,
                                 &pipelineInfo,
                                 nullptr,
                                 &computePipeline->handle) != VK_SUCCESS) {
        throw std::runtime_error("Could not create render pipeline!");
    }

    return computePipeline;
}

ComputePipeline::~ComputePipeline() {
    vkDestroyPipeline(device->getHandle(), handle, nullptr);
}
