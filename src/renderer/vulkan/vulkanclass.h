//
// Created by nic on 18/06/2021.
//

#pragma once

#ifndef CUBICAD_RENDER_VULKAN_VULKANCLASS_H_
#define CUBICAD_RENDER_VULKAN_VULKANCLASS_H_

#include <vulkan/vulkan.h>


template<typename T>
class VulkanClass {
  protected:
    T handle = VK_NULL_HANDLE;

  public:
    T getHandle() const { return handle; }

    T *getHandlePtr() { return &handle; }

    //virtual ~VulkanClass() = default;
};

#endif //CUBICAD_RENDER_VULKAN_VULKANCLASS_H_
