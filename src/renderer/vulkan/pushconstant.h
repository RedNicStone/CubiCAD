//
// Created by nic on 29/05/2021.
//

#ifndef CUBICAD_RENDER_VULKAN_PUSHCONSTANT_H_
#define CUBICAD_RENDER_VULKAN_PUSHCONSTANT_H_

#include <vulkan/vulkan.h>

#include "vulkanclass.h"


class PushConstantRange : public VulkanClass<VkPushConstantRange> {
  public:
    template<class type>
    PushConstantRange();
};

#endif //CUBICAD_RENDER_VULKAN_PUSHCONSTANT_H_
