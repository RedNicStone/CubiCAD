//
// Created by nic on 18/06/2021.
//

#pragma once

#ifndef CUBICAD_RENDER_VULKAN_VULKANCLASS_H_
#define CUBICAD_RENDER_VULKAN_VULKANCLASS_H_

#include <vulkan/vulkan.h>

/// this class is a parent class for all vulkan object wrapping classes
/// \tparam T vulkan class that is being wrapped
template<typename T>
class VulkanClass {
  protected:
    T handle = VK_NULL_HANDLE; //< native vulkan handle

  public:
    /// function to acquire native vulkan handle
    /// \return vulkan handle
    T getHandle() const { return handle; }

    /// function to acquire pointer to native vulkan handle (unsafe)
    /// \return pointer to vulkan handle
    T *getHandlePtr() { return &handle; }

    //virtual ~VulkanClass() = default;
};

#endif //CUBICAD_RENDER_VULKAN_VULKANCLASS_H_
