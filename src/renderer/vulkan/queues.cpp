//
// Created by nic on 18/06/2021.
//

#include "queues.h"


void QueueFamilyHandler::findOptimalQueues() {
    std::vector<size_t> viability_mask(possibleFamilies.size());
    for (size_t i = 0; i < possibleFamilies.size(); i++) {
        viability_mask[i] = std::bitset<sizeof(VkQueueFlags)>(possibleFamilies[i].flags).count();
    }
    for (auto &pair : optimalFamilies) {
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

QueueFamilyHandler::QueueFamilyHandler(PhysicalDevice *pDevice) : device(pDevice) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->getHandle(), &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device->getHandle(), &queueFamilyCount, queueFamilies.data());

    possibleFamilies.reserve(queueFamilyCount);
    for (const auto &queueFamily : queueFamilies) {
        PossibleQueueFamily family{};
        family.flags = queueFamily.queueFlags;
        family.maxQueueCount = queueFamily.queueCount;
        possibleFamilies.push_back(family);
    }

    findOptimalQueues();
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

optional<uint32_t> QueueFamilyHandler::getOptimalPresentQueue(Window *window) {
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

bool QueueFamilyHandler::queryPresetSupport(Window *window, uint32_t queueFamily) {
    VkBool32 presentSupport;
    vkGetPhysicalDeviceSurfaceSupportKHR(device->getHandle(), queueFamily, window->getSurface(), &presentSupport);
    return presentSupport;
}

std::vector<VkDeviceQueueCreateInfo> QueueFamilyHandler::getCreateInfo() {
    std::vector<VkDeviceQueueCreateInfo> createInfos{};
    createInfos.reserve(families.size());

    for (auto & family : families)
        createInfos.push_back(family.getCreateInfo());

    return createInfos;
}

uint32_t QueueFamilyHandler::getEnablesQueueFamilyCount() {
    uint32_t enabledQueueFamilies = 0;
    for (auto &possibleFamily : possibleFamilies)
        if (possibleFamily.queueCount)
            enabledQueueFamilies++;
    return enabledQueueFamilies;
}

void QueueFamilyHandler::createQueues() {
    for (size_t i = 0; i < possibleFamilies.size(); i++)
        if (possibleFamilies[i].queueCount)
            families.emplace_back(this, i, possibleFamilies[i].queueCount);
}

void QueueFamilyHandler::getQueues(Device *pDevice) {
    for (auto &family : families)
        family.getQueues(pDevice);
}

void Queue::submitCommandBuffer(std::vector<VkSubmitInfo> submitInfos, Fence* fence) {
    if (useLock) {
        std::cerr << "submitting work failed, queue already has work submitted. *the snake bit its tail*" << std::endl;
        return;
    }

    useLock = true;

    auto submissions = static_cast<uint32_t>(submitInfos.size());
    vkQueueSubmit(handle, submissions, submitInfos.data(), fence->getHandle());

    auto lambda = [&]() {
      vkQueueWaitIdle(handle);
      useLock = false;
    };

    std::thread waitingThread(lambda);
    waitingThread.detach();
}

bool Queue::hasWorkSubmitted() const {
    return useLock;
}

Queue::Queue(QueueFamily *family) {
    queueFamily = family;
}

QueueFamily::QueueFamily(QueueFamilyHandler *pFamilyHandler, uint32_t family, uint32_t count) : familyHandler(
    pFamilyHandler), queueFamily(family), queueCount(count) {}

VkDeviceQueueCreateInfo QueueFamily::getCreateInfo() const {
    float queue_priority = 1.0f;

    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queueFamily;
    queue_create_info.queueCount = queueCount;
    queue_create_info.pQueuePriorities = &queue_priority;

    return queue_create_info;
}

void QueueFamily::getQueues(Device *pDevice) {
    for (uint32_t i = 0; i < queueCount; i++) {
        queues.emplace_back(this);
        vkGetDeviceQueue(pDevice->getHandle(), queueFamily, i, queues[i].getHandlePtr());
    }
}
