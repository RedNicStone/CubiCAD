//
// Created by nic on 30/12/2021.
//

#include "material.h"

#include <utility>


std::shared_ptr<Material> Material::create(const std::shared_ptr<MasterMaterial>& pMasterMaterial, const std::string&
pName) {
    auto material = std::make_shared<Material>();
    material->masterMaterial = pMasterMaterial;

    if (pName.empty())
        material->name = pMasterMaterial->generateMaterialName();
    else
        material->name = pName;

    return  material;
}
