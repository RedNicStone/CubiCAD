//
// Created by nic on 03/07/2021.
//

#include "commandpool.h"


CommandPool::CommandPool(Device *pDevice, Queue *pQueue, VkCommandPoolCreateFlags flags)
    : device(pDevice), queue(pQueue) {

    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = queue->getQueueFamilyIndex();
    create_info.flags = flags;

    if (vkCreateCommandPool(device->getHandle(), &create_info, nullptr, &handle) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool");
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(device->getHandle(), handle, nullptr);
}
