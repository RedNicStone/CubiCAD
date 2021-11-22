//
// Created by nic on 20/09/2021.
//

#include "fence.h"


Fence::Fence(Device *pDevice, bool signaled) {
    device = pDevice;

    VkFenceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    vkCreateFence(device->getHandle(), &info, nullptr, &handle);
}

bool Fence::getState() {
    return vkGetFenceStatus(device->getHandle(), handle) == VK_SUCCESS;
}

void Fence::resetState() {
    vkResetFences(device->getHandle(), 1, &handle);
}

void Fence::waitForSignal(uint64_t timeout) {
    VkFence waitFences[] =  { handle };
    vkWaitForFences(device->getHandle(), 1, waitFences, VK_TRUE, timeout);
}

Fence::~Fence() {
    vkDestroyFence(device->getHandle(), handle, nullptr);
}
