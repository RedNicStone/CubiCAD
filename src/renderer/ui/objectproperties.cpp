//
// Created by nic on 21/01/2022.
//

#include "objectproperties.h"


std::shared_ptr<ObjectProperties> ObjectProperties::create(const std::shared_ptr<Scene> &pScene) {
    auto objectProperties = std::make_shared<ObjectProperties>();
    objectProperties->scene = pScene;

    return objectProperties;
}

void ObjectProperties::setObjectByID(uint32_t vObjectID) {
    if (vObjectID > 0) {
        if (static_cast<int>(vObjectID) != objectID) {
            objectID = static_cast<int>(vObjectID);
            object = scene->getInstanceByID(vObjectID);
            objectName = object->getName();
            pos = object->getPosition();
            rot = object->getRotation();
            scale = object->getScale();
            meshName = object->getMesh()->getName();
            selectedMeshlet = 0;
            selectedMaterial = 0;
        }
    } else
        object = nullptr;
}

void ObjectProperties::drawUI() {
    if (firstStartup) {
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        firstStartup = false;
    }

    ImGuiWindowFlags windowFlags{};
    if (!ImGui::Begin("Object Property Editor", nullptr, windowFlags)) {
        ImGui::End();
        return;
    }

    if (object != nullptr) {
        ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("Properties", tabFlags)) {
            if (ImGui::BeginTabItem("Object")) {
                ImGui::BeginDisabled();
                {
                    ImGui::DragInt("Object ID", &objectID);
                }
                ImGui::EndDisabled();
                if (ImGui::InputText("Object Name",
                                     &objectName,
                                     ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
                    object->setName(objectName);

                float speed = 0.01f;
                if (ImGui::DragFloat3("Position", &pos.x, speed))
                    object->setPosition(pos);
                if (ImGui::DragFloat3("Rotation", &rot.x, speed))
                    object->setRotation(rot);
                if (ImGui::DragFloat3("Scale", &scale.x, speed))
                    object->setScale(scale);

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Mesh")) {
                if (ImGui::InputText("Mesh Name",
                                     &meshName,
                                     ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
                    object->getMesh()->setName(meshName);
                auto temp = static_cast<int>(object->getMesh()->getVertexCount());
                ImGui::DragInt("Vertices", &temp);

                ImGui::Separator();
                ImGui::Text("Meshlets");

                const auto &meshlets = object->getMesh()->getMeshlets();
                for (uint32_t i = 0; i < meshlets.size(); i++) {
                    if (ImGui::Selectable(("Meshlet with "
                        + std::to_string(meshlets[i]->indexData.size())
                        + " vertices").c_str(), i == selectedMeshlet)) {
                        selectedMeshlet = i;
                    }
                }

                ImGui::Separator();
                ImGui::Text("Meshlet Properties");

                if (selectedMeshlet < meshlets.size()) {
                    ImGui::BeginDisabled();
                    {
                        temp = static_cast<int>(meshlets[selectedMeshlet]->indexData.size());
                        ImGui::DragInt("Indices", &temp);
                        std::string materialName = "Testing material #1";
                        ImGui::InputText("Material", &materialName);
                    }
                    ImGui::EndDisabled();
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Material")) {
                std::unordered_map<std::shared_ptr<MasterMaterial>, std::vector<std::shared_ptr<Material>>> materials{};

                uint32_t id = 0;
                for (const auto &masterMaterial: materials) {
                    ImGuiTreeNodeFlags
                        base_flags =
                        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

                    ImGuiTreeNodeFlags root_flags{};
                    if (selectedMaterial == id)
                        root_flags = ImGuiTreeNodeFlags_Selected;

                    if (ImGui::TreeNodeEx(masterMaterial.first->getName().c_str(), root_flags)) {
                        id++;

                        for (const auto &material: masterMaterial.second) {
                            ImGuiTreeNodeFlags node_flags = base_flags;
                            if (id == selectedMaterial)
                                node_flags |= ImGuiTreeNodeFlags_Selected;

                            ImGui::TreeNodeEx(material->getName().c_str(), node_flags);
                            id++;

                            if (ImGui::IsItemClicked())
                                selectedMaterial = id;
                        }

                        ImGui::TreePop();
                    }
                    if (ImGui::IsItemClicked())
                        selectedMaterial = id;
                }

                ImGui::Separator();
                ImGui::Text("Material Properties");

                ImGui::TextColored(ImVec4(199.0f, 0.0f, 57.0f, 1.0f), "Work in progress");

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    } else {
        ImGui::Text("No object selected");
    }
    ImGui::End();
}
