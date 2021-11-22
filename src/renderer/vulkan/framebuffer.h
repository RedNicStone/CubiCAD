//
// Created by nic on 03/08/2021.
//

#pragma once

#ifndef CUBICAD_FRAMEBUFFER_H
#define CUBICAD_FRAMEBUFFER_H

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkanclass.h"
#include "device.h"
#include "renderpass.h"
#include "imageview.h"
#include "swapchain.h"
#include "semaphore.h"


class RenderPass;

class SwapChain;

class FrameBuffer {
  private:
    Device *device;

    std::vector<VkFramebuffer> framebuffers;

  public:
    FrameBuffer(Device *device,
                RenderPass *renderPass,
                VkExtent2D extent,
                std::vector<std::vector<ImageView>> &imageViews);

    FrameBuffer(Device *device,
                RenderPass *renderPass,
                SwapChain *swapChain,
                std::vector<std::vector<ImageView>> &imageViews);

    VkFramebuffer getFramebuffer(uint32_t i) { return framebuffers[i]; }

    ~FrameBuffer();
};

#endif //CUBICAD_FRAMEBUFFER_H
