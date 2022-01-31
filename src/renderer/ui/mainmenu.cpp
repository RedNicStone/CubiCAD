//
// Created by nic on 27/01/2022.
//

#include "mainmenu.h"


std::shared_ptr<MainMenu> MainMenu::create(const std::shared_ptr<TextureLibrary>& textureLibrary,
                                           const std::shared_ptr<SceneWriter>& sceneWriter) {
    auto menubar = std::make_shared<MainMenu>();
    menubar->sceneWriter = sceneWriter;

    menubar->logoTexture = textureLibrary->createTexture("resources/icons/cubicad_logo_rv1_452x128.png",
                                                         VK_FORMAT_R8G8B8A8_UNORM);

    menubar->logoDescriptorSet = ImGui_ImplVulkan_AddTexture(textureLibrary->getSampler()->getHandle(),
                                                             menubar->logoTexture->getImageView()->getHandle(),
                                                             menubar->logoTexture->getImage()->getLayout());

    return menubar;
}

void MainMenu::drawUI() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::ImageButton((ImTextureID) logoDescriptorSet, { 452 / 128.0f * FONT_SIZE, FONT_SIZE }))
            showAboutWindow =! showAboutWindow;

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "CTRL+N")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Open", "CTRL+O")) {
                sceneWriter->readScene();
            }
            if (ImGui::BeginMenu("Open Recent")) {
                ImGui::MenuItem("fish_hat.c");
                ImGui::MenuItem("fish_hat.inl");
                ImGui::MenuItem("fish_hat.h");
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "CTRL+S")) {
                sceneWriter->writeScene();
            }
            if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Instance"))
        {
            if (ImGui::MenuItem("Import from obj", "CTRL+I")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Duplicate", "CTRL+D")) {}
            if (ImGui::MenuItem("Delete", "DELETE")) {}
            ImGui::Separator();
            if (ImGui::BeginMenu("Add Primitive")) {
                ImGui::MenuItem("Cube");
                ImGui::MenuItem("Sphere");
                ImGui::MenuItem("Cone");
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showAboutWindow)
        drawAboutWindow();
}

void MainMenu::drawAboutWindow() {
    ImGuiWindowFlags windowFlags{};
    if (!ImGui::Begin("About CubiCAD", nullptr, windowFlags)) {
        ImGui::End();
        return;
    }

    ImGui::SameLine((ImGui::GetWindowWidth() / 2) - (452.0f / 2));
    ImGui::Image((ImTextureID) logoDescriptorSet, { 452, 128 });
    ImGui::Separator();
    ImGui::Text("CubiCAD is a vulkan-based 3D engine written by Nicholas Karwath (aka. RedNicStone)\n\n"
                "Thanks to everyone who contributed to these libraries (In no specific order):\n"
                "- Doxygen & Doxygen Awesome Css\n"
                "- GLFW\n"
                "- Fork Awesome & Icon Font Cpp Headers\n"
                "- Dear ImGui\n"
                "- libMorton\n"
                "- rapidjson\n"
                "- shaderc\n"
                "- stb\n"
                "- tinyobjloader\n"
                "- Vulkan & Vulkan Memory Allocator\n");

    ImGui::End();
}
