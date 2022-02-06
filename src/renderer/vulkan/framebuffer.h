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

class ImageView;

class FrameBuffer {
  private:
    std::shared_ptr<Device> device;

    std::vector<VkFramebuffer> frameBuffers;
    VkExtent2D extent;

  public:
    static std::shared_ptr<FrameBuffer> create(const std::shared_ptr<Device> &pDevice,
                                               const std::shared_ptr<RenderPass> &renderPass,
                                               VkExtent2D vExtent,
                                               std::vector<std::vector<std::shared_ptr<ImageView>>> &imageViews);

    static std::shared_ptr<FrameBuffer> create(std::shared_ptr<Device> pDevice,
                                               const std::shared_ptr<RenderPass> &renderPass,
                                               const std::shared_ptr<SwapChain> &swapChain,
                                               std::vector<std::vector<std::shared_ptr<ImageView>>> &imageViews);

    VkFramebuffer getFramebuffer(uint32_t i) { return frameBuffers[i]; }

    VkExtent2D getExtent() { return extent; }

    ~FrameBuffer();
};

#endif //CUBICAD_FRAMEBUFFER_H
