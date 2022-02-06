//
// Created by nic on 27/01/2022.
//

#include "mainmenu.h"


std::shared_ptr<MainMenu> MainMenu::create(const std::shared_ptr<RenderManager>& renderManager) {
    NFD_Init();

    auto menubar = std::make_shared<MainMenu>();
    menubar->renderManager = renderManager;

    menubar->logoTexture = renderManager->getTextureLibrary()->createTexture("resources/icons/cubicad_logo_rv1_452x128.png",
                                                         VK_FORMAT_R8G8B8A8_UNORM);

    menubar->logoDescriptorSet = ImGui_ImplVulkan_AddTexture(renderManager->getTextureLibrary()->getSampler()->getHandle(),
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
                renderManager->getSceneWriter()->readScene();
            }
            if (ImGui::BeginMenu("Open Recent")) {
                ImGui::MenuItem("fish_hat.c");
                ImGui::MenuItem("fish_hat.inl");
                ImGui::MenuItem("fish_hat.h");
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "CTRL+S")) {
                renderManager->getSceneWriter()->writeScene();
            }
            if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Instance"))
        {
            if (ImGui::MenuItem(OPEN_OBJ_FILE_DIALOG_TITLE, "CTRL+I"))
                showOpenOBJFileDialog = true;
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

    if (showOpenOBJFileDialog) {
        ImGui::OpenPopup(OPEN_OBJ_FILE_DIALOG_TITLE);
        showOpenOBJFileDialog = false;
    }

    if (openOBJFileDialog())
        ImGui::CloseCurrentPopup();
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

bool MainMenu::openOBJFileDialog() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, {0.5f, 0.5f});
    if (ImGui::BeginPopupModal(OPEN_OBJ_FILE_DIALOG_TITLE, nullptr,
                           ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::InputText("##filename", &lastFileName);
        ImGui::SameLine();
        if (ImGui::Button(ICON_FK_FOLDER_OPEN)) {
            nfdchar_t* outPath;
            nfdfilteritem_t filterItem[1] = {{"Wavefront files", "obj"}};
            nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, nullptr);

            if (result == NFD_OKAY) {
                lastFileName = outPath;
                NFD_FreePath(outPath);
            }
        }
        if (ImGui::Button("Load object")) {
            renderManager->loadMesh(lastFileName);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}
