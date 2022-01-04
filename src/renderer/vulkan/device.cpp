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

std::shared_ptr<Device> Device::create(const std::shared_ptr<PhysicalDevice> &physicalDevice,
                                       std::vector<const char *> deviceExtensions,
                                       const VkPhysicalDeviceFeatures &deviceFeatures) {
    auto device = std::make_shared<Device>();
    device->physicalDevice = physicalDevice;

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

    if (vkCreateDevice(physicalDevice->getHandle(), &createInfo, nullptr, &device->handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    physicalDevice->getQueueFamilyHandler()->getQueues(device);

    device->createMemoryAllocator();

    return device;
}

Device::~Device() {
    vkDestroyDevice(handle, nullptr);
}

void Device::waitIdle() {
    vkDeviceWaitIdle(handle);
}
