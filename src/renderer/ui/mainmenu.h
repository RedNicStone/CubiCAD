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

    std::vector<std::string> lastFileNames;

    bool showAboutWindow;
    bool showOpenOBJFileDialog;
    bool showConfirmDiscardDialog;

    static const constexpr char *OPEN_OBJ_FILE_DIALOG_TITLE = "Import from OBJ";
    static const constexpr char *CONFIRM_DISCARD_DIALOG_TITLE = "Confirm to discard changes";

    void drawAboutWindow();

    bool openOBJFileDialog();
    void confirmDiscardDialog();

  public:
    static std::shared_ptr<MainMenu> create(const std::shared_ptr<RenderManager> &renderManager);

    void drawUI() override;
};

#endif //CUBICAD_MAINMENU_H
