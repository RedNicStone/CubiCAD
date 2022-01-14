//
// Created by nic on 03/07/2021.
//

#include "commandpool.h"


std::shared_ptr<CommandPool> CommandPool::create(const std::shared_ptr<Device> &pDevice,
                                                 const std::shared_ptr<Queue> &pQueue,
                                                 VkCommandPoolCreateFlags flags) {
    auto commandPool = std::make_shared<CommandPool>();

    commandPool->device = pDevice;
    commandPool->queue = pQueue;

    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = pQueue->getQueueFamilyIndex();
    create_info.flags = flags;

    if (vkCreateCommandPool(pDevice->getHandle(), &create_info, nullptr, &commandPool->handle) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool");

    return commandPool;
}

void CommandPool::reset() {
    vkResetCommandPool(device->getHandle(), handle, 0);
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(device->getHandle(), handle, nullptr);
}
