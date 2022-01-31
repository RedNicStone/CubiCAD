//
// Created by nic on 27/01/2022.
//

#ifndef CUBICAD_MAINMENU_H
#define CUBICAD_MAINMENU_H

#include <IconsForkAwesome.h>

#include "../scene.h"
#include "../scenewriter.h"
#include "uirenderer.h"
#include "../texture.h"


class MainMenu : public UIDrawable {
  private:
    std::shared_ptr<SceneWriter> sceneWriter;

    std::shared_ptr<Texture> logoTexture;
    VkDescriptorSet logoDescriptorSet;

    bool showAboutWindow;

    void drawAboutWindow();

  public:
    static std::shared_ptr<MainMenu> create(const std::shared_ptr<TextureLibrary>& textureLibrary,
                                            const std::shared_ptr<SceneWriter>& sceneWriter);

    void drawUI() override;
};

#endif //CUBICAD_MAINMENU_H
