//
// Created by nic on 14/06/2021.
//

#pragma once

#ifndef CUBICAD_RENDER_VULKAN_PHYSICALDEVICE_H_
#define CUBICAD_RENDER_VULKAN_PHYSICALDEVICE_H_

#include <vulkan/vulkan_core.h>

#include <string>
#include <set>
#include <iostream>
#include <bitset>
#include <vector>
#include <memory>

#include "types.h"
#include "vulkanclass.h"
#include "instance.h"
#include "queues.h"


class Instance;

class QueueFamilyHandler;

class PhysicalDevice : public VulkanClass<VkPhysicalDevice> {
  private:
    Instance *instance;
    std::shared_ptr<QueueFamilyHandler> queueFamilyHandler;

  public:
    explicit PhysicalDevice(Instance *callingInstance, VkPhysicalDevice device);

    QueueFamilyHandler *getQueueFamilyHandler() { return queueFamilyHandler.get(); }

    Instance *getInstance();

    bool hasRequiredExtensions(const std::vector<std::string> &requiredExtensions);
    std::vector<VkExtensionProperties> getSupportedExtensions();

    void queryDeviceProperties();

};

#endif //CUBICAD_RENDER_VULKAN_PHYSICALDEVICE_H_
