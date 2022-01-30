//
// Created by nic on 22/01/2022.
//

#pragma once

#ifndef CUBICAD_OBJECTLIST_H
#define CUBICAD_OBJECTLIST_H

#include "../scene.h"
#include "uirenderer.h"


class ObjectList : public UIDrawable {
  private:
    std::shared_ptr<Scene> scene;

    bool firstStartup = true;
    uint32_t itemSelected = 0;
    uint32_t itemHovered = 0;

    void updateSelected();

  public:
    static std::shared_ptr<ObjectList> create(const std::shared_ptr<Scene>& pScene);

    void drawUI() override;
};

#endif //CUBICAD_OBJECTLIST_H
