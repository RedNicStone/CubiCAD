//
// Created by nic on 14/03/2021.
//

#pragma once

#ifndef CUBICAD_WINDOW_H
#define CUBICAD_WINDOW_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

#include "instance.h"


class Instance;

class Window {
  private:
    Instance *instance;

    GLFWwindow *window = nullptr;
    VkSurfaceKHR surface{};
    VkExtent2D extend{};
    bool resized = false;

    static void staticResizeCallback(GLFWwindow *window, int width, int height);
    void dynamicResizeCallback(int width, int height);

  public:
    Window(Instance *pInstance, const std::string &title, int width, int height);

    bool getResized() const { return resized; }

    void setResized(bool resized_) { resized = resized_; }

    void resetResizeFence() { resized = false; }

    VkSurfaceKHR getSurface() { return surface; }

    GLFWwindow *getWindow() { return window; }

    VkExtent2D getSurfaceExtend() { return extend; }

    static void initWindows();

    ~Window();
};

#endif //CUBICAD_WINDOW_H
