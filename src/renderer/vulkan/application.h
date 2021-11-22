//
// Created by nic on 14/03/2021.
//

#ifndef CUBICAD_APPLICATION_H
#define CUBICAD_APPLICATION_H

#include "device.h"
#include "graphicspipeline.h"
#include "window.h"
#include "swapchain.h"
#include "instance.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>


class VulkanRenderApplication {
  private:
    VkDebugUtilsMessengerEXT debugMessenger;

    Device device;
    Instance instance;
    GraphicsPipeline pipeline;
    Window window;
    SwapChain *swapChain;

    void framebufferResizeCallback(uint32_t width, uint32_t height);

  public:
    void createWindow(const std::string &title, uint32_t width, uint32_t height);

};

#endif //CUBICAD_APPLICATION_H
