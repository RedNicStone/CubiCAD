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

class PhysicalDevice;

class QueueFamilyHandler : public std::enable_shared_from_this<QueueFamilyHandler> {
  private:
    std::shared_ptr<PhysicalDevice> device;

    std::vector<PossibleQueueFamily> possibleFamilies;
    std::vector<std::shared_ptr<QueueFamily>> families;

    std::map<VkQueueFlagBits, optional<uint32_t>>
        optimalFamilies = {{VK_QUEUE_GRAPHICS_BIT, {}}, {VK_QUEUE_COMPUTE_BIT, {}}, {VK_QUEUE_TRANSFER_BIT, {}}};

    optional<uint32_t> presentFamily;

    void findOptimalQueues();

  public:
    static std::shared_ptr<QueueFamilyHandler> create(std::shared_ptr<PhysicalDevice> pDevice);
    void createQueues();
    void getQueues(const std::shared_ptr<Device> &pDevice);

    std::shared_ptr<PhysicalDevice> getPhysicalDevice() { return device; }

    optional<uint32_t> getOptimalPresentQueue(const std::shared_ptr<Window> &window);
    optional<uint32_t> getOptimalQueueFamily(VkQueueFlagBits flag);
    uint32_t getMaximumQueueCount(uint32_t queueFamily);
    bool queryQueueFamilySupport(VkQueueFlagBits flag);
    bool queryPresetSupport(const std::shared_ptr<Window> &window, uint32_t queueFamily);

    uint32_t registerQueueFamilyAllocation(uint32_t queueFamily, uint32_t queue_count);
    void appendQueueFamilyAllocations(uint32_t queueFamily, uint32_t queue_count);
    bool getRegisteredQueueFamilyAllocationCount(uint32_t queueFamily);
    std::vector<uint32_t> getRegisteredQueueFamilies();
    uint32_t getEnablesQueueFamilyCount();
    std::vector<VkDeviceQueueCreateInfo> getCreateInfo();

    std::shared_ptr<QueueFamily> getQueueFamily(uint32_t queueFamily) { return families[queueFamily]; }
};

class Queue;

class Fence;

class QueueFamily : public std::enable_shared_from_this<QueueFamily> {
  private:
    std::weak_ptr<QueueFamilyHandler> familyHandler;

    std::vector<std::shared_ptr<Queue>> queues;
    uint32_t queueFamily;
    uint32_t queueCount;

  public:
    static std::shared_ptr<QueueFamily> create(const std::shared_ptr<QueueFamilyHandler> &pFamilyHandler,
                                               uint32_t family,
                                               uint32_t count);

    [[nodiscard]] uint32_t getQueueFamilyIndex() const { return queueFamily; }

    [[nodiscard]] VkDeviceQueueCreateInfo getCreateInfo() const;
    void getQueues(const std::shared_ptr<Device> &pDevice);

    std::weak_ptr<QueueFamilyHandler> getHandler() { return familyHandler; };

    std::shared_ptr<Queue> getQueue(uint32_t id) { return queues[id]; }
};

class Queue : public VulkanClass<VkQueue> {
  private:
    std::shared_ptr<QueueFamily> queueFamily;

    bool useLock{false};

  public:
    static std::shared_ptr<Queue> create(const std::shared_ptr<QueueFamily> &family);

    std::shared_ptr<QueueFamily> getQueueFamily() { return queueFamily; }

    uint32_t getQueueFamilyIndex() { return queueFamily->getQueueFamilyIndex(); }

    void submitCommandBuffer(std::vector<VkSubmitInfo> submitInfos, const std::shared_ptr<Fence> &fence = nullptr);
    [[nodiscard]] bool hasWorkSubmitted() const;
    void waitForIdle();
};

#endif //CUBICAD_RENDER_VULKAN_QUEUES_H_
