//
// Created by nic on 30/12/2021.
//

#include "material.h"

#include <utility>


std::shared_ptr<Material> Material::create(std::shared_ptr<MasterMaterial> pMasterMaterial) {
    auto material = std::make_shared<Material>();
    material->masterMaterial = std::move(pMasterMaterial);

    return  material;
}
