//
// Created by nic on 08/03/2021.
//


#define VMA_IMPLEMENTATION

#include <vulkan/vulkan_core.h>

#include "device.h"


void Device::createMemoryAllocator() {
    VmaAllocatorCreateInfo createInfo{};
    createInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    createInfo.instance = physicalDevice->getInstance()->getHandle();
    createInfo.physicalDevice = physicalDevice->getHandle();
    createInfo.device = handle;

    vmaCreateAllocator(&createInfo, &allocator);
}

Device::Device(PhysicalDevice *physicalDevice,
               std::vector<const char *> deviceExtensions,
               const VkPhysicalDeviceFeatures &deviceFeatures) : physicalDevice(physicalDevice) {

    physicalDevice->getQueueFamilyHandler()->createQueues();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = physicalDevice->getQueueFamilyHandler()->getCreateInfo();

    std::vector<char *> validationLayers = physicalDevice->getInstance()->getValidationLayers();

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    if (vkCreateDevice(physicalDevice->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    physicalDevice->getQueueFamilyHandler()->getQueues(this);

    createMemoryAllocator();
}

void Device::cleanup() {
    vkDestroyDevice(handle, nullptr);
}
