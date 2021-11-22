//
// Created by nic on 14/03/2021.
//

#include "window.h"


void Window::staticResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    app->dynamicResizeCallback(width, height);
}

void Window::dynamicResizeCallback(int width, int height) {
    extend = {(uint32_t) width, (uint32_t) height};

    resized = true;
}

void Window::initWindows() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

Window::Window(Instance *pInstance, const std::string &title, int width, int height) {
    instance = pInstance;

    window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, staticResizeCallback);

    glfwGetWindowSize(window, (int *) &extend.width, (int *) &extend.height);

    if (glfwCreateWindowSurface(instance->getHandle(), window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

Window::~Window() {
    glfwDestroyWindow(window);
    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
}
