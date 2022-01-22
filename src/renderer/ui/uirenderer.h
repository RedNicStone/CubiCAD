//
// Created by nic on 19/01/2022.
//

#pragma once

#ifndef CUBICAD_UIRENDERER_H
#define CUBICAD_UIRENDERER_H

#include "../vulkan/descriptorset.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>


class UIDrawable {
  public:
    virtual void drawUI() {};

    virtual ~UIDrawable()=default;
};

class UIRenderer {
  private:
    std::shared_ptr<DescriptorPool> ImGUIPool;

    std::vector<std::shared_ptr<UIDrawable>> drawables;

    ImGuiContext* context = nullptr;
    bool hideUI = false;

  public:
    static std::shared_ptr<UIRenderer> create(const std::shared_ptr<Queue>& graphicsQueue,
                                       const std::shared_ptr<CommandPool>& transferPool,
                                       const std::shared_ptr<RenderPass>& renderPass,
                                       const std::shared_ptr<Window>& window,
                                       uint32_t imageCount,
                                       uint32_t subpass = 0);

    void draw(const std::shared_ptr<CommandBuffer>& graphicsCommandBuffer);

    void setHidden(bool hide);

    static ImGuiIO& getIO() { return ImGui::GetIO(); }
    [[nodiscard]] bool getHidden() const { return hideUI; }

    ~UIRenderer();
};

#endif //CUBICAD_UIRENDERER_H
