//
// Created by nic on 21/01/2022.
//

#pragma once

#ifndef CUBICAD_OBJECTPROPERTIES_H
#define CUBICAD_OBJECTPROPERTIES_H

#include <imgui.h>

#include "uirenderer.h"
#include "../scene.h"


class ObjectProperties : public UIDrawable {
  private:
    std::shared_ptr<Scene> scene;

    std::shared_ptr<MeshInstance> object;

    std::string objectName;

  public:
    static std::shared_ptr<ObjectProperties> create(const std::shared_ptr<Scene>& pScene);

    void setObjectByID(uint32_t objectID);

    void drawUI() override;
};

#endif //CUBICAD_OBJECTPROPERTIES_H
