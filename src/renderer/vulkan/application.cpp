//
// Created by nic on 14/03/2021.
//

#include "application.h"


void VulkanRenderApplication::createWindow(const std::string &title, uint32_t width, uint32_t height) {
    Window::initWindows();

    window = Window(&instance, title, static_cast<int>(width), static_cast<int>(height));
}
