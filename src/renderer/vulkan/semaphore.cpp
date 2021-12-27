//
// Created by nic on 22/08/2021.
//

#include "semaphore.h"

#include <utility>


std::shared_ptr<Semaphore> Semaphore::create(std::shared_ptr<Device> pDevice) {
    auto semaphore = std::make_shared<Semaphore>();
    semaphore->device = std::move(pDevice);

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(semaphore->device->getHandle(), &createInfo, nullptr, &semaphore->handle);

    return semaphore;
}

Semaphore::~Semaphore() {
    vkDestroySemaphore(device->getHandle(), handle, nullptr);
}
