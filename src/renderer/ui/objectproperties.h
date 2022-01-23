//
// Created by nic on 21/01/2022.
//

#pragma once

#ifndef CUBICAD_OBJECTPROPERTIES_H
#define CUBICAD_OBJECTPROPERTIES_H

#include <imgui.h>
#include <imgui_stdlib.h>

#include "uirenderer.h"
#include "../scene.h"


class ObjectProperties : public UIDrawable {
  private:
    std::shared_ptr<Scene> scene;

    std::shared_ptr<MeshInstance> object = nullptr;
    bool firstStartup = true;

    int objectID;
    std::string objectName;
    std::string meshName;
    glm::vec3 pos = {0, 0, 0};
    glm::vec3 rot = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};
    uint32_t selectedMeshlet = 0;
    uint32_t selectedMaterial = 0;

  public:
    static std::shared_ptr<ObjectProperties> create(const std::shared_ptr<Scene>& pScene);

    void setObjectByID(uint32_t vObjectID);

    void drawUI() override;
};

#endif //CUBICAD_OBJECTPROPERTIES_H
