//
// Created by nic on 18/06/2021.
//

#include "queues.h"

#include <utility>


void QueueFamilyHandler::findOptimalQueues() {
    std::vector<size_t> viability_mask(possibleFamilies.size());
    for (size_t i = 0; i < possibleFamilies.size(); i++) {
        viability_mask[i] = std::bitset<sizeof(VkQueueFlags)>(possibleFamilies[i].flags).count();
    }
    for (auto &pair: optimalFamilies) {
        uint32_t queueID = 0;
        queueID = 0;
        size_t min_viability = SIZE_MAX;
        for (size_t i = 0; i < possibleFamilies.size(); i++)
            if (//possibleFamilies[i].maxQueueCount > 0 &&
                possibleFamilies[i].flags & pair.first && viability_mask[i] < min_viability) {
                queueID = static_cast<uint32_t>(i);
                min_viability = viability_mask[i];
            }
        if (min_viability != SIZE_MAX)
            pair.second = queueID;
    }
}

std::shared_ptr<QueueFamilyHandler> QueueFamilyHandler::create(std::shared_ptr<PhysicalDevice> pDevice) {
    auto queueFamilyHandler = std::make_shared<QueueFamilyHandler>();
    queueFamilyHandler->device = std::move(pDevice);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(queueFamilyHandler->device->getHandle(), &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(queueFamilyHandler->device->getHandle(),
                                             &queueFamilyCount,
                                             queueFamilies.data());

    queueFamilyHandler->possibleFamilies.reserve(queueFamilyCount);
    for (const auto &queueFamily: queueFamilies) {
        PossibleQueueFamily family{};
        family.flags = queueFamily.queueFlags;
        family.maxQueueCount = queueFamily.queueCount;
        queueFamilyHandler->possibleFamilies.push_back(family);
    }

    queueFamilyHandler->findOptimalQueues();

    return queueFamilyHandler;
}

optional<uint32_t> QueueFamilyHandler::getOptimalQueueFamily(VkQueueFlagBits flag) {
    auto family_index = optimalFamilies.find(flag);
    if (family_index->second.has_value())
        return family_index->second;
    for (size_t i = 0; i > possibleFamilies.size(); i++)
        if (possibleFamilies[i].flags & flag)
            return {};
    return {};
}

uint32_t QueueFamilyHandler::getMaximumQueueCount(uint32_t queueFamily) {
    return possibleFamilies[queueFamily].maxQueueCount;
}

bool QueueFamilyHandler::queryQueueFamilySupport(VkQueueFlagBits flag) {
    return std::any_of(possibleFamilies.begin(), possibleFamilies.end(), [=](PossibleQueueFamily &family) {
      return family.flags & flag;
    });
}

uint32_t QueueFamilyHandler::registerQueueFamilyAllocation(uint32_t queueFamily, uint32_t queue_count) {
    possibleFamilies[queueFamily].queueCount = queue_count;
    return queueFamily - 1;
}

void QueueFamilyHandler::appendQueueFamilyAllocations(uint32_t queueFamily, uint32_t queue_count) {
    possibleFamilies[queueFamily].queueCount += queue_count;
}

bool QueueFamilyHandler::getRegisteredQueueFamilyAllocationCount(uint32_t queueFamily) {
    return possibleFamilies[queueFamily].queueCount;
}

std::vector<uint32_t> QueueFamilyHandler::getRegisteredQueueFamilies() {
    std::vector<uint32_t> enabled_queue_families{};
    for (uint32_t i = 0; i < possibleFamilies.size(); i++)
        if (possibleFamilies[i].queueCount)
            enabled_queue_families.push_back(i);
    return enabled_queue_families;
}

optional<uint32_t> QueueFamilyHandler::getOptimalPresentQueue(const std::shared_ptr<Window> &window) {
    auto graphicsFamily = optimalFamilies.find(VK_QUEUE_GRAPHICS_BIT);
    if (graphicsFamily->second.has_value() && queryPresetSupport(window, graphicsFamily->second.value()))
        return graphicsFamily->second;
    optional<uint32_t> queue;
    for (size_t i = 0; i < possibleFamilies.size(); i++) {
        if (//possibleFamilies[i].maxQueueCount > 0 &&
            queryPresetSupport(window, static_cast<uint32_t>(i))) {
            auto queueID = static_cast<uint32_t>(i);
            queue = queueID;
            return queue;
        }
    }
    return {};
}

bool QueueFamilyHandler::queryPresetSupport(const std::shared_ptr<Window> &window, uint32_t queueFamily) {
    VkBool32 presentSupport;
    vkGetPhysicalDeviceSurfaceSupportKHR(device->getHandle(), queueFamily, window->getSurface(), &presentSupport);
    return presentSupport;
}

std::vector<VkDeviceQueueCreateInfo> QueueFamilyHandler::getCreateInfo() {
    std::vector<VkDeviceQueueCreateInfo> createInfos{};
    createInfos.reserve(families.size());

    for (auto &family: families)
        createInfos.push_back(family->getCreateInfo());

    return createInfos;
}

uint32_t QueueFamilyHandler::getEnablesQueueFamilyCount() {
    uint32_t enabledQueueFamilies = 0;
    for (auto &possibleFamily: possibleFamilies)
        if (possibleFamily.queueCount)
            enabledQueueFamilies++;
    return enabledQueueFamilies;
}

void QueueFamilyHandler::createQueues() {
    for (uint32_t i = 0; i < possibleFamilies.size(); i++)
        if (possibleFamilies[i].queueCount)
            families.push_back(QueueFamily::create(shared_from_this(),
                                                   i,
                                                   possibleFamilies[i].queueCount));
}

void QueueFamilyHandler::getQueues(const std::shared_ptr<Device> &pDevice) {
    for (auto &family: families)
        family->getQueues(pDevice);
}

void Queue::submitCommandBuffer(std::vector<VkSubmitInfo> submitInfos, const std::shared_ptr<Fence> &fence) {
    if (useLock) {
        vkQueueWaitIdle(handle);
        useLock = false;
    }

    useLock = true;
    auto submissions = static_cast<uint32_t>(submitInfos.size());
    vkQueueSubmit(handle, submissions, submitInfos.data(), fence->getHandle());
}

bool Queue::hasWorkSubmitted() const {
    return useLock;
}

std::shared_ptr<Queue> Queue::create(const std::shared_ptr<QueueFamily> &family) {
    auto queue = std::make_shared<Queue>();
    queue->queueFamily = family;

    return queue;
}

void Queue::waitForIdle() {
    if (useLock) {
        vkQueueWaitIdle(handle);
        useLock = false;
    }
}

std::shared_ptr<QueueFamily> QueueFamily::create(const std::shared_ptr<QueueFamilyHandler> &pFamilyHandler,
                                                 uint32_t family,
                                                 uint32_t count) {
    auto queueFamily = std::make_shared<QueueFamily>();
    queueFamily->familyHandler = pFamilyHandler;
    queueFamily->queueFamily = family;
    queueFamily->queueCount = count;

    return queueFamily;
}

VkDeviceQueueCreateInfo QueueFamily::getCreateInfo() const {
    float queue_priority = 1.0f;

    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queueFamily;
    queue_create_info.queueCount = queueCount;
    queue_create_info.pQueuePriorities = &queue_priority;

    return queue_create_info;
}

void QueueFamily::getQueues(const std::shared_ptr<Device> &pDevice) {
    for (uint32_t i = 0; i < queueCount; i++) {
        queues.push_back(Queue::create(shared_from_this()));
        vkGetDeviceQueue(pDevice->getHandle(), queueFamily, i, queues[i]->getHandlePtr());
    }
}
