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

    std::vector<std::string> lastImports;
    std::vector<std::string> lastFiles;

    float maxAnisotropy;

    bool showAboutWindow;
    bool showOpenOBJFileDialog;
    bool showOpenSaveFileDialog;
    bool showSelectSaveFileDialog;
    bool showConfirmDiscardDialog;
    bool showSettingsDialog;

    static const constexpr char *OPEN_OBJ_FILE_DIALOG_TITLE = "Import from OBJ";
    static const constexpr char *OPEN_SAVE_FILE_DIALOG_TITLE = "Open save file";
    static const constexpr char *SELECT_SAVE_FILE_DIALOG_TITLE = "Select save file";
    static const constexpr char *CONFIRM_DISCARD_DIALOG_TITLE = "Confirm to discard changes";

    void drawAboutWindow();
    void drawSettingsWindow();

    bool openOBJFileDialog();
    bool openSaveFileDialog();
    bool selectSaveFileDialog();
    void confirmDiscardDialog();

  public:
    static std::shared_ptr<MainMenu> create(const std::shared_ptr<RenderManager> &renderManager);

    void drawUI() override;
};

#endif //CUBICAD_MAINMENU_H
