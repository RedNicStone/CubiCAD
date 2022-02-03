//
// Created by nic on 30/12/2021.
//

#include "material.h"

#include <utility>


std::shared_ptr<Material> Material::create(const std::shared_ptr<MasterMaterial>& pMasterMaterial,
                                           const std::shared_ptr<DescriptorPoolManager>& descriptorManager,
                                           const std::vector<std::shared_ptr<Texture>>& textures,
                                           void *parameters,
                                           const std::string& pName) {
    auto material = std::make_shared<Material>();
    material->masterMaterial = pMasterMaterial;
    material->parameters = parameters;

    material->materialSet = descriptorManager->allocate(pMasterMaterial->getMaterialDescriptorSetLayout());
    for (uint32_t i = 0; i < textures.size(); i++) {
        material->materialSet->updateImage(textures[i]->getImageView(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, i + 1);
    }

    if (pName.empty())
        material->name = pMasterMaterial->generateMaterialName();
    else
        material->name = pName;

    return  material;
}

void Material::updateParameterBuffer(const std::shared_ptr<Buffer>& buffer) {
    materialSet->updateUniformBuffer(buffer, 0, masterMaterial->getParameterSize(), parameterBufferOffset);
}

Material::~Material() {
    delete static_cast<char*>(parameters);
}
