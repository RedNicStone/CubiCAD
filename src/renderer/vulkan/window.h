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

/// class that handles window creation, resizing and destruction
class Window {
  private:
    std::shared_ptr<Instance> instance; //< instance that owns the window

    GLFWwindow *window = nullptr; //< pointer to glfw window
    VkSurfaceKHR surface{}; //< vulkan handle to the physical window surface
    VkExtent2D extend{}; //< extend of the window surface
    bool resized = false; //< boolean to store if the window has resized

    void *userPtr = nullptr; //< user pointer to be used for glfw interrupt callbacks

    /// callback for when the window has been resized. Sets resized to true
    /// \param window pointer to the glfw window
    /// \param width new width
    /// \param height new height
    static void staticResizeCallback(GLFWwindow *window, int width, int height);

    /// 
    /// \param width
    /// \param height
    void dynamicResizeCallback(int width, int height);

  public:
    static std::shared_ptr<Window> create(std::shared_ptr<Instance> pInstance,
                                          const std::string &title,
                                          int width,
                                          int height);

    [[nodiscard]] bool getResized() const { return resized; }

    void setResized(bool resized_) { resized = resized_; }

    void resetResizeFence() { resized = false; }

    VkSurfaceKHR getSurface() { return surface; }

    GLFWwindow *getWindow() { return window; }

    VkExtent2D getSurfaceExtend() { return extend; }

    void setUserPointer(void *pointer) { userPtr = pointer; }

    void *getUserPointer() { return userPtr; }

    static void initWindows();

    ~Window();
};

#endif //CUBICAD_WINDOW_H
