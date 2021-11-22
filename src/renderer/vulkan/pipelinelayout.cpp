//
// Created by nic on 22/05/2021.
//

#include "pipelinelayout.h"


PipelineLayout::PipelineLayout(Device *pDevice,
                               std::vector<DescriptorSetLayout> &descriptorLayouts,
                               std::vector<PushConstantRange> &pushConstants) : device(pDevice) {
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayout> descriptorLayoutHandles(descriptorLayouts.size());
    descriptorLayoutHandles.resize(descriptorLayouts.size());
    for (size_t i = 0; i < descriptorLayouts.size(); ++i)
        descriptorLayoutHandles[i] = descriptorLayouts[i].getHandle();
    createInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayoutHandles.size());
    createInfo.pSetLayouts = descriptorLayoutHandles.data();

    std::vector<VkPushConstantRange> pushConstantRangeHandles(pushConstants.size());
    pushConstantRangeHandles.resize(pushConstants.size());
    for (size_t i = 0; i < pushConstants.size(); ++i)
        pushConstantRangeHandles[i] = pushConstants[i].getHandle();
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRangeHandles.size());
    createInfo.pPushConstantRanges = pushConstantRangeHandles.data();

    if (vkCreatePipelineLayout(device->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

PipelineLayout::PipelineLayout(Device *pDevice, std::vector<DescriptorSetLayout> &descriptorLayouts) : device
(pDevice) {
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayout> descriptorLayoutHandles(descriptorLayouts.size());
    descriptorLayoutHandles.resize(descriptorLayouts.size());
    for (size_t i = 0; i < descriptorLayouts.size(); ++i)
        descriptorLayoutHandles[i] = descriptorLayouts[i].getHandle();
    createInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayoutHandles.size());
    createInfo.pSetLayouts = descriptorLayoutHandles.data();

    createInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(device->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance pipeline layout!");
    }
}

PipelineLayout::PipelineLayout(Device *pDevice, std::vector<PushConstantRange> &pushConstants) : device(pDevice) {
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    createInfo.setLayoutCount = 0;

    std::vector<VkPushConstantRange> pushConstantRangeHandles(pushConstants.size());
    pushConstantRangeHandles.resize(pushConstants.size());
    for (size_t i = 0; i < pushConstants.size(); ++i)
        pushConstantRangeHandles[i] = pushConstants[i].getHandle();
    createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRangeHandles.size());
    createInfo.pPushConstantRanges = pushConstantRangeHandles.data();

    if (vkCreatePipelineLayout(device->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance pipeline layout!");
    }
}

PipelineLayout::PipelineLayout(Device *pDevice) : device(pDevice) {
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    createInfo.setLayoutCount = 0;
    createInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(device->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance pipeline layout!");
    }
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(device->getHandle(), handle, nullptr);
}
