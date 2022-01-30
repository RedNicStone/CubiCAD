//
// Created by nic on 22/01/2022.
//

#include "objectlist.h"


std::shared_ptr<ObjectList> ObjectList::create(const std::shared_ptr <Scene> &pScene) {
    auto objectList = std::make_shared<ObjectList>();
    objectList->scene = pScene;

    return objectList;
}

void ObjectList::drawUI() {
    updateSelected();

    if (firstStartup) {
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        firstStartup = false;
    }

    ImGuiWindowFlags windowFlags{};
    if (!ImGui::Begin("Object List", nullptr, windowFlags)) {
        ImGui::End();
        return;
    }

    std::unordered_map<std::shared_ptr<Mesh>,
                       std::vector<std::shared_ptr<MeshInstance>>> meshes{};
    for (const auto& instance : scene->getInstances())
        meshes[instance->getMesh()].push_back(instance);

    for (const auto& mesh : meshes) {
        ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen |
            ImGuiTreeNodeFlags_Bullet;

        itemSelected = 0;
        itemHovered = 0;
        if (ImGui::TreeNode(mesh.first->getName().c_str())) {
            for (const auto &instance : mesh.second) {
                ImGuiTreeNodeFlags node_flags = base_flags;
                if (scene->getSelected() == instance->getID())
                    node_flags |= ImGuiTreeNodeFlags_Selected;

                ImGui::TreeNodeEx(instance->getName().c_str(), node_flags);

                if (ImGui::IsItemClicked())
                    itemSelected = instance->getID();
                if (ImGui::IsItemHovered())
                    itemHovered = instance->getID();
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void ObjectList::updateSelected() {
    if (itemSelected > 0)
        scene->setSelected(itemSelected);
    if (itemHovered > 0)
        scene->setHovered(itemHovered);
}
