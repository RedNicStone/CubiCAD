//
// Created by nic on 18/06/2021.
//

#pragma once

#ifndef CUBICAD_RENDER_VULKAN_QUEUES_H_
#define CUBICAD_RENDER_VULKAN_QUEUES_H_

#include <vulkan/vulkan_core.h>

#include <vector>
#include <map>
#include <algorithm>
#include <optional>
#include <bitset>
#include <thread>
#include <mutex>

#include "vulkanclass.h"
#include "device.h"
#include "physicaldevice.h"
#include "window.h"
#include "fence.h"


struct PossibleQueueFamily {
    uint32_t maxQueueCount;
    VkQueueFlags flags;
    uint32_t queueCount;
};

class Window;

class QueueFamily;

class Device;

class QueueFamilyHandler {
  private:
    PhysicalDevice *device;

    std::vector<PossibleQueueFamily> possibleFamilies;
    std::vector<QueueFamily> families;

    std::map<VkQueueFlagBits, optional<uint32_t>>
        optimalFamilies = {{VK_QUEUE_GRAPHICS_BIT, {}}, {VK_QUEUE_COMPUTE_BIT, {}}, {VK_QUEUE_TRANSFER_BIT, {}}};

    optional<uint32_t> presentFamily;

    void findOptimalQueues();

  public:
    explicit QueueFamilyHandler(PhysicalDevice *pDevice);
    void createQueues();
    void getQueues(Device *pDevice);

    PhysicalDevice *getPhysicalDevice() { return device; }

    optional<uint32_t> getOptimalPresentQueue(Window *window);
    optional<uint32_t> getOptimalQueueFamily(VkQueueFlagBits flag);
    uint32_t getMaximumQueueCount(uint32_t queueFamily);
    bool queryQueueFamilySupport(VkQueueFlagBits flag);
    bool queryPresetSupport(Window *window, uint32_t queueFamily);

    uint32_t registerQueueFamilyAllocation(uint32_t queueFamily, uint32_t queue_count);
    void appendQueueFamilyAllocations(uint32_t queueFamily, uint32_t queue_count);
    bool getRegisteredQueueFamilyAllocationCount(uint32_t queueFamily);
    std::vector<uint32_t> getRegisteredQueueFamilies();
    uint32_t getEnablesQueueFamilyCount();
    std::vector<VkDeviceQueueCreateInfo> getCreateInfo();

    QueueFamily *getQueueFamily(uint32_t queueFamily) { return &families[queueFamily]; }
};

class Queue;

class Fence;

class QueueFamily {
  private:
    QueueFamilyHandler *familyHandler;

    std::vector<Queue> queues;
    uint32_t queueFamily;
    uint32_t queueCount;

  public:
    QueueFamily(QueueFamilyHandler *pFamilyHandler, uint32_t family, uint32_t count);

    uint32_t getQueueFamilyIndex() const { return queueFamily; }

    VkDeviceQueueCreateInfo getCreateInfo() const;
    void getQueues(Device *pDevice);

    QueueFamilyHandler *getHandler() { return familyHandler; };

    Queue *getQueue(uint32_t id) { return &queues[id]; }
};

class Queue : public VulkanClass<VkQueue> {
  private:
    QueueFamily *queueFamily;

    bool useLock{false};

  public:
    explicit Queue(QueueFamily *family);

    QueueFamily *getQueueFamily() { return queueFamily; }

    uint32_t getQueueFamilyIndex() { return queueFamily->getQueueFamilyIndex(); }

    void submitCommandBuffer(std::vector<VkSubmitInfo> submitInfos, Fence* fence);
    bool hasWorkSubmitted() const;
};

#endif //CUBICAD_RENDER_VULKAN_QUEUES_H_
