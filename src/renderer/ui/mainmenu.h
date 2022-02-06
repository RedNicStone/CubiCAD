//
// Created by nic on 27/01/2022.
//

#ifndef CUBICAD_MAINMENU_H
#define CUBICAD_MAINMENU_H

#include <IconsForkAwesome.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <nfd.h>

#include "../scene.h"
#include "../scenewriter.h"
#include "uirenderer.h"
#include "../texture.h"


class MainMenu : public UIDrawable {
  private:
    std::shared_ptr<RenderManager> renderManager;

    std::shared_ptr<Texture> logoTexture;
    VkDescriptorSet logoDescriptorSet;

    std::string lastFileName;

    bool showAboutWindow;
    bool showOpenOBJFileDialog;

    static const constexpr char *OPEN_OBJ_FILE_DIALOG_TITLE = "Import from OBJ";

    void drawAboutWindow();

  public:
    static std::shared_ptr<MainMenu> create(const std::shared_ptr<RenderManager> &renderManager);

    bool openOBJFileDialog();

    void drawUI() override;
};

#endif //CUBICAD_MAINMENU_H
