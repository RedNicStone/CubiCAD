//
// Created by nic on 08/03/2021.
//

#include "computepipeline.h"


std::shared_ptr<ComputePipeline> ComputePipeline::create(const std::shared_ptr<Device>& pDevice,
                                                           const std::shared_ptr<PipelineLayout>& pLayout,
                                                           const std::shared_ptr<ComputeShader> &shader) {
    auto graphicsPipeline = std::make_shared<ComputePipeline>();
    graphicsPipeline->device = pDevice;
    graphicsPipeline->layout = pLayout;

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage = shader->pipelineStageInfo();
    pipelineInfo.layout = graphicsPipeline->layout->getHandle();
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateComputePipelines(graphicsPipeline->device->getHandle(),
                                  VK_NULL_HANDLE,
                                  1,
                                  &pipelineInfo,
                                  nullptr,
                                  &graphicsPipeline->handle) != VK_SUCCESS) {
        throw std::runtime_error("Could not create render pipeline!");
    }

    return graphicsPipeline;
}

ComputePipeline::~ComputePipeline() {
    vkDestroyPipeline(device->getHandle(), handle, nullptr);
}
