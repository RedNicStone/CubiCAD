//
// Created by nic on 22/08/2021.
//

#include "semaphore.h"


Semaphore::Semaphore(Device *pDevice) : device(pDevice) {
    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(device->getHandle(), &createInfo, nullptr, &handle);
}
