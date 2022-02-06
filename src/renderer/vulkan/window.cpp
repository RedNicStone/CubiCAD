//
// Created by nic on 14/03/2021.
//

#include "window.h"

#include <utility>


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

std::shared_ptr<Window> Window::create(std::shared_ptr<Instance> pInstance,
                                       const std::string &title,
                                       int width,
                                       int height) {
    auto window = std::make_shared<Window>();
    window->instance = std::move(pInstance);

    window->window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    glfwSetWindowUserPointer(window->window, window.get());
    glfwSetFramebufferSizeCallback(window->window, staticResizeCallback);

    glfwGetWindowSize(window->window, (int *) &window->extend.width, (int *) &window->extend.height);

    if (glfwCreateWindowSurface(window->instance->getHandle(), window->window, nullptr, &window->surface)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    return window;
}

Window::~Window() {
    glfwDestroyWindow(window);
    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
}
