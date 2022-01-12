//
// Created by nic on 21/05/2021.
//

#include <utility>

#include "shader.h"
#include "../../utils/utils.h"


std::shared_ptr<Shader> Shader::create(std::shared_ptr<Device> pDevice,
                                       std::string nName,
                                       const std::string &filename) {
    auto shader = std::make_shared<Shader>();
    shader->device = std::move(pDevice);
    shader->name = std::move(nName);
    shader->shaderCode = Utils::readFile(filename);
    shader->loadModule();

    return shader;
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
