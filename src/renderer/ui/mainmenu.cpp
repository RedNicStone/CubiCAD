//
// Created by nic on 27/01/2022.
//

#include "mainmenu.h"


std::shared_ptr<MainMenu> MainMenu::create(const std::shared_ptr<RenderManager> &renderManager) {
    NFD_Init();

    auto menubar = std::make_shared<MainMenu>();
    menubar->renderManager = renderManager;
    menubar->lastImports = {""};
    menubar->lastFiles = {""};

    menubar->logoTexture =
        renderManager->getTextureLibrary()
            ->createTexture("resources/icons/cubicad_logo_rv1_452x128.png", VK_FORMAT_R8G8B8A8_UNORM);

    menubar->logoDescriptorSet =
        ImGui_ImplVulkan_AddTexture(renderManager->getTextureLibrary()->getSampler()->getHandle(),
                                    menubar->logoTexture->getImageView()->getHandle(),
                                    menubar->logoTexture->getImage()->getLayout());

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(renderManager->getDevice()->getPhysicalDevice()->getHandle(), &properties);
    menubar->maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    return menubar;
}

void MainMenu::drawUI() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::ImageButton((ImTextureID) logoDescriptorSet, {452 / 128.0f * FONT_SIZE, FONT_SIZE}))
            showAboutWindow = !showAboutWindow;

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "CTRL+N")) {
                showConfirmDiscardDialog = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open", "CTRL+O")) {
                showOpenSaveFileDialog = true;
            }
            if (ImGui::BeginMenu("Open Recent")) {
                for (size_t i = 0; i < lastFiles.size() && i < 20; i++) {
                    if (!lastFiles[lastFiles.size() - i - 1].empty()) {
                        auto meshFile = std::filesystem::path(lastFiles[lastFiles.size() - i - 1]);
                        if (ImGui::MenuItem(meshFile.filename().c_str())) {
                            if (i != 0)
                                lastFiles.push_back(lastFiles[lastFiles.size() - i - 1]);
                            showOpenSaveFileDialog = true;
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "CTRL+S")) {
                if (lastFiles.back().empty())
                    showSelectSaveFileDialog = true;
                else
                    renderManager->getSceneWriter()->writeScene();
            }
            if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S")) {
                showSelectSaveFileDialog = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Settings", "CTRL+ALT+S")) {
                showSettingsDialog = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Instance")) {
            if (ImGui::MenuItem(OPEN_OBJ_FILE_DIALOG_TITLE, "CTRL+I"))
                showOpenOBJFileDialog = true;
            if (ImGui::BeginMenu("Import recent")) {
                for (size_t i = 0; i < lastImports.size() && i < 20; i++) {
                    if (!lastImports[lastImports.size() - i - 1].empty()) {
                        auto meshFile = std::filesystem::path(lastImports[lastImports.size() - i - 1]);
                        if (ImGui::MenuItem(meshFile.filename().c_str())) {
                            if (i != 0)
                                lastImports.push_back(lastImports[lastImports.size() - i - 1]);
                            showOpenOBJFileDialog = true;
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Duplicate", "CTRL+D")) {
                if (renderManager->getScene()->getSelected() != 0) {
                    auto instance = renderManager->getScene()->getInstanceByID(renderManager->getScene()->getSelected
                        ());
                    renderManager->getScene()->submitInstance(MeshInstance::create(instance->getMesh(), instance->getName()));
                }
            }
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

    if (showOpenSaveFileDialog) {
        ImGui::OpenPopup(OPEN_SAVE_FILE_DIALOG_TITLE);
        showOpenSaveFileDialog = false;
    }

    if (showSelectSaveFileDialog) {
        ImGui::OpenPopup(SELECT_SAVE_FILE_DIALOG_TITLE);
        showSelectSaveFileDialog = false;
    }

    if (showConfirmDiscardDialog) {
        ImGui::OpenPopup(CONFIRM_DISCARD_DIALOG_TITLE);
        showConfirmDiscardDialog = false;
    }

    if (showSettingsDialog)
        drawSettingsWindow();

    openOBJFileDialog();
    openSaveFileDialog();
    selectSaveFileDialog();
    confirmDiscardDialog();
}

void MainMenu::drawAboutWindow() {
    ImGuiWindowFlags windowFlags{};
    if (!ImGui::Begin("About CubiCAD", nullptr, windowFlags)) {
        ImGui::End();
        return;
    }

    ImGui::SameLine((ImGui::GetWindowWidth() / 2) - (452.0f / 2));
    ImGui::Image((ImTextureID) logoDescriptorSet, {452, 128});
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

void MainMenu::drawSettingsWindow() {
    ImGuiWindowFlags windowFlags{};
    if (!ImGui::Begin("Scene Settings", nullptr, windowFlags)) {
        ImGui::End();
        return;
    }

    TextureQualitySettings settings = renderManager->getTextureLibrary()->getTextureSettings();
    ImGui::Text("Texture settings");
    if (ImGui::SliderFloat("Anisotropy", &settings.anisotropy, 1.0, maxAnisotropy, "ratio = %.1f")) {
        renderManager->registerFunctionNextFrame([=](){
            renderManager->waitForExecution();
            auto temp = renderManager->getTextureLibrary()->getSampler();
            renderManager->getTextureLibrary()->updateSettings(settings);
            renderManager->getMaterialLibrary()->updateImageSampler();
        });
    }

    ImGui::End();
}

bool MainMenu::openOBJFileDialog() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, {0.5f, 0.5f});
    if (ImGui::BeginPopupModal(OPEN_OBJ_FILE_DIALOG_TITLE,
                               nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::InputText("##filename", &(lastImports.back()));
        ImGui::SameLine();
        if (ImGui::Button(ICON_FK_FOLDER_OPEN)) {
            nfdchar_t *outPath;
            nfdfilteritem_t filterItem[1] = {{"Wavefront files", ".obj"}};
            const char *defaultPath = lastImports[0].c_str();
            nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, defaultPath);

            if (result == NFD_OKAY) {
                if (lastImports.back() != outPath)
                    lastImports.emplace_back(outPath);
                NFD_FreePath(outPath);
            }
        }
        if (ImGui::Button("Load object")) {
            renderManager->loadMesh(lastImports.back());
            renderManager->getScene()->bakeMaterials(true);
            renderManager->getScene()->collectRenderBuffers();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

bool MainMenu::openSaveFileDialog() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, {0.5f, 0.5f});
    if (ImGui::BeginPopupModal(OPEN_SAVE_FILE_DIALOG_TITLE,
                               nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::InputText("##filename", &(lastFiles.back()));
        ImGui::SameLine();
        if (ImGui::Button(ICON_FK_FOLDER_OPEN)) {
            nfdchar_t *outPath;
            nfdfilteritem_t filterItem[2] = {{"CubiCAD saves", ".ccs"},
                                             {"Compressed CubiCAD saves", ".ccs.lz4"}};
            const char *defaultPath = lastFiles[0].c_str();
            nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, defaultPath);

            if (result == NFD_OKAY) {
                if (lastFiles.back() != outPath)
                    lastFiles.emplace_back(outPath);
                NFD_FreePath(outPath);
            }
        }
        if (ImGui::Button("Load file")) {
            renderManager->getSceneWriter()->setFilename(lastFiles.back());
            renderManager->getScene()->clear();
            renderManager->getSceneWriter()->readScene();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

bool MainMenu::selectSaveFileDialog() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, {0.5f, 0.5f});
    if (ImGui::BeginPopupModal(SELECT_SAVE_FILE_DIALOG_TITLE,
                               nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::InputText("##filename", &(lastFiles.back()));
        ImGui::SameLine();
        if (ImGui::Button(ICON_FK_FOLDER_OPEN)) {
            nfdchar_t *outPath;
            nfdfilteritem_t filterItem[2] = {{"CubiCAD saves", ".ccs"},
                                             {"Compressed CubiCAD saves", ".ccs.lz4"}};
            const char *defaultPath = lastFiles[0].c_str();
            nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 2, defaultPath, "");

            if (result == NFD_OKAY) {
                if (lastFiles.back() != outPath)
                    lastFiles.emplace_back(outPath);
                NFD_FreePath(outPath);
            }
        }
        if (ImGui::Button("Load file")) {
            renderManager->getSceneWriter()->setFilename(lastFiles.back());
            renderManager->getSceneWriter()->writeScene();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

void MainMenu::confirmDiscardDialog() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, {0.5f, 0.5f});
    if (ImGui::BeginPopupModal(CONFIRM_DISCARD_DIALOG_TITLE,
                               nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("Are you sure you want to discard all unsaved changes?");
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Button]                = ImVec4(0.92f, 0.23f, 0.08f, 0.49f);
        style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.97f, 0.25f, 0.10f, 0.68f);
        style.Colors[ImGuiCol_ButtonActive]          = ImVec4(1.00f, 0.30f, 0.15f, 1.00f);
        if (ImGui::Button("Discard changes")) {
            renderManager->getScene()->clear();
            ImGui::CloseCurrentPopup();
        }
        style.Colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}
