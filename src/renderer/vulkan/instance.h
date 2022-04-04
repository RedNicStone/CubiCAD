//
// Created by nic on 21/03/2021.
//

#pragma once

#ifndef CUBICAD_MESHINSTANCE_H
#define CUBICAD_INSTANCE_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cstring>
#include <iostream>

#include "physicaldevice.h"


static VKAPI_ATTR VKAPI_CALL VkBool32 debugCallback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    [[maybe_unused]] void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

class PhysicalDevice;

class Instance : public VulkanClass<VkInstance>, public std::enable_shared_from_this<Instance> {
  private:
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkApplicationInfo appInfo{};

    std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices;
    std::vector<char *, std::allocator<char *>> validationLayers;

    bool enableValidationLayers;

    void createPhysicalDevices();

    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                 VkDebugUtilsMessengerEXT *pDebugMessenger);

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    static bool checkExtensionSupport(bool validationLayerExtensions);

    static std::vector<VkLayerProperties> getSupportedLayers();
    static bool checkValidationLayerSupport(const std::vector<char *> &validationLayers);

    static std::vector<const char *> getRequiredExtensions(bool validationLayerExtensions);

  public:
    static std::shared_ptr<Instance> create(const char *appName,
                                            uint32_t appVersion,
                                            bool shouldEnableValidationLayers);

    std::vector<std::shared_ptr<PhysicalDevice>> &getPhysicalDevices() { return physicalDevices; }

    const char *getName() const { return appInfo.pApplicationName; }

    std::vector<char *> getValidationLayers() { return validationLayers; }

    [[nodiscard]] std::vector<const char *> getExtensions() const { return getRequiredExtensions(enableValidationLayers); }
};

#endif //CUBICAD_MESHINSTANCE_H
