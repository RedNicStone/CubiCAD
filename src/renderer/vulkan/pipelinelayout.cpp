//
// Created by nic on 22/05/2021.
//

#include "pipelinelayout.h"

#include <utility>


std::shared_ptr<PipelineLayout> PipelineLayout::create(const std::shared_ptr<Device>& pDevice,
                                                       const std::vector<std::shared_ptr<DescriptorSetLayout>>
                                                       &descriptorLayouts,
                                                       const std::vector<std::shared_ptr<PushConstantRange>>
                                                       &pushConstants) {
    auto pipelineLayout = std::make_shared<PipelineLayout>();
    pipelineLayout->device = pDevice;

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayout> descriptorLayoutHandles(descriptorLayouts.size());
    descriptorLayoutHandles.resize(descriptorLayouts.size());
    for (size_t i = 0; i < descriptorLayouts.size(); ++i)
        descriptorLayoutHandles[i] = descriptorLayouts[i]->getHandle();
    createInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayoutHandles.size());
    createInfo.pSetLayouts = descriptorLayoutHandles.data();

    std::vector<VkPushConstantRange> pushConstantRangeHandles(pushConstants.size());
    pushConstantRangeHandles.resize(pushConstants.size());
    for (size_t i = 0; i < pushConstants.size(); ++i)
        pushConstantRangeHandles[i] = pushConstants[i]->getHandle();
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRangeHandles.size());
    createInfo.pPushConstantRanges = pushConstantRangeHandles.data();

    if (vkCreatePipelineLayout(pipelineLayout->device->getHandle(), &createInfo, nullptr, &pipelineLayout->handle)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    return pipelineLayout;
}

std::shared_ptr<PipelineLayout> PipelineLayout::create(const std::shared_ptr<Device>& pDevice,
                                                       const std::vector<std::shared_ptr<DescriptorSetLayout>>
                                                       &descriptorLayouts) {
    auto pipelineLayout = std::make_shared<PipelineLayout>();
    pipelineLayout->device = pDevice;

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayout> descriptorLayoutHandles(descriptorLayouts.size());
    descriptorLayoutHandles.resize(descriptorLayouts.size());
    for (size_t i = 0; i < descriptorLayouts.size(); ++i)
        descriptorLayoutHandles[i] = descriptorLayouts[i]->getHandle();
    createInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayoutHandles.size());
    createInfo.pSetLayouts = descriptorLayoutHandles.data();

    createInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(pipelineLayout->device->getHandle(), &createInfo, nullptr, &pipelineLayout->handle)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance pipeline layout!");
    }

    return pipelineLayout;
}

std::shared_ptr<PipelineLayout> PipelineLayout::create(const std::shared_ptr<Device>& pDevice,
                                                       const std::vector<std::shared_ptr<PushConstantRange>>
                                                       &pushConstants) {
    auto pipelineLayout = std::make_shared<PipelineLayout>();
    pipelineLayout->device = pDevice;

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    createInfo.setLayoutCount = 0;

    std::vector<VkPushConstantRange> pushConstantRangeHandles(pushConstants.size());
    pushConstantRangeHandles.resize(pushConstants.size());
    for (size_t i = 0; i < pushConstants.size(); ++i)
        pushConstantRangeHandles[i] = pushConstants[i]->getHandle();
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRangeHandles.size());
    createInfo.pPushConstantRanges = pushConstantRangeHandles.data();

    if (vkCreatePipelineLayout(pipelineLayout->device->getHandle(), &createInfo, nullptr, &pipelineLayout->handle)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance pipeline layout!");
    }

    return pipelineLayout;
}

std::shared_ptr<PipelineLayout> PipelineLayout::create(std::shared_ptr<Device> pDevice) {
    auto pipelineLayout = std::make_shared<PipelineLayout>();
    pipelineLayout->device = std::move(pDevice);

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    createInfo.setLayoutCount = 0;
    createInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(pipelineLayout->device->getHandle(), &createInfo, nullptr, &pipelineLayout->handle)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance pipeline layout!");
    }

    return pipelineLayout;
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(device->getHandle(), handle, nullptr);
}
