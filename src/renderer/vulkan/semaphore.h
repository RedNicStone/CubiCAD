//
// Created by nic on 22/08/2021.
//

#pragma once

#ifndef CUBICAD_SEMAPHORE_H
#define CUBICAD_SEMAPHORE_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "vulkanclass.h"
#include "device.h"


class Semaphore : public VulkanClass<VkSemaphore> {
  private:
    Device *device;

  public:
    Semaphore(Device *pDevice);

};

#endif //CUBICAD_SEMAPHORE_H
