//
// Created by nic on 21/05/2021.
//

#include "shader.h"

#include <utility>


Shader::Shader(Device *device, std::string name, const std::string &filename) : name(std::move(name)), device(device) {
    shaderCode = readFile(filename);
    loadModule();
}

void Shader::loadModule() {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(shaderCode.data());

    if (vkCreateShaderModule(device->getHandle(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
}

VkPipelineShaderStageCreateInfo Shader::pipelineStageInfo() {
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = shaderStage;
    createInfo.module = shaderModule;
    createInfo.pName = name.data();

    return createInfo;
}

Shader::~Shader() {
    vkDestroyShaderModule(device->getHandle(), shaderModule, nullptr);
}
