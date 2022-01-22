//
// Created by nic on 21/01/2022.
//

#include "objectproperties.h"


std::shared_ptr<ObjectProperties> ObjectProperties::create(const std::shared_ptr<Scene>& pScene) {
    auto objectProperties = std::make_shared<ObjectProperties>();
    objectProperties->scene = pScene;

    return objectProperties;
}

void ObjectProperties::setObjectByID(uint32_t objectID) {
    object = scene->getInstanceByID(objectID);
    objectName = object->getName();
}

void ObjectProperties::drawUI() {
    ImGui::SetNextWindowSize(ImVec2(200, 150));
    ImGui::Begin("Object Property Editor");
    ImGui::Text("Object Name: ");
    if (ImGui::InputText("Object Name: ",
                         &objectName, 50,
                         ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
        object->setName(objectName);
    ImGui::End();
}
