//
// Created by nic on 09/03/2021.
//

#pragma once

#ifndef CUBICAD_SWAPCHAIN_H
#define CUBICAD_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <dexode/EventBus.hpp>

#include <vector>
#include <stdexcept>

#include "window.h"
#include "framebuffer.h"
#include "../eventbus.h"
#include "semaphore.h"
#include "commandbuffer.h"


class CommandBuffer;

class SwapChain {
  private:
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;

    std::shared_ptr<Queue> presentQueue;
    uint32_t imageCount;
    VkFormat imageFormat;
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    uint32_t currentFrame{0};
    uint32_t currentImageIndex{0};
    std::vector<std::shared_ptr<Semaphore>> imageAvailableSemaphores;
    std::vector<std::shared_ptr<Semaphore>> renderFinishedSemaphores;
    std::vector<std::shared_ptr<Fence>> inFlightFences;
    std::vector<std::shared_ptr<Fence>> imagesInFlight;

    std::shared_ptr<Device> device;
    std::shared_ptr<Window> window;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat();
    VkPresentModeKHR chooseSwapPresentMode();

    void createSyncObjects();

    void recreateSwapChain();

  public:
    void acquireNextFrame();
    void presentImage();
    //construction
    static std::shared_ptr<SwapChain> create(const std::shared_ptr<Device> &pDevice,
                                             const std::shared_ptr<Window> &pWindow,
                                             const std::shared_ptr<Queue> &pQueue,
                                             uint32_t imageCount_v,
                                             std::vector<uint32_t> &accessingQueues);

    VkSurfaceCapabilitiesKHR querySurfaceCapabilities(VkPhysicalDevice *physicalDevice);
    std::vector<VkSurfaceFormatKHR> querySurfaceFormats(VkPhysicalDevice *physicalDevice);
    std::vector<VkPresentModeKHR> queryPresentModes(VkPhysicalDevice *physicalDevice);

    VkSwapchainKHR getHandle() { return swapChain; }

    VkFormat getFormat() { return imageFormat; }

    VkImage getSwapChainImage(uint32_t i) { return swapChainImages[i]; }

    VkImageView getSwapChainImageView(uint32_t i) { return swapChainImageViews[i]; }

    VkExtent2D getSwapExtent();

    [[nodiscard]] uint32_t getImageCount() const { return imageCount; }

    [[nodiscard]] uint32_t getCurrentFrame() const { return currentFrame; }

    [[nodiscard]] uint32_t getCurrentImageIndex() const { return currentImageIndex; }

    [[nodiscard]] uint32_t *getCurrentImageIndex_() { return &currentImageIndex; }

    std::vector<std::shared_ptr<Semaphore>> getRenderSignalSemaphores() {
        return {renderFinishedSemaphores[currentFrame]};
    }

    std::vector<std::shared_ptr<Semaphore>> getRenderWaitSemaphores() {
        return {imageAvailableSemaphores[currentFrame]};
    }

    std::shared_ptr<Fence> getPresentFence() { return inFlightFences[currentFrame]; }

    ~SwapChain();
};

#endif //CUBICAD_SWAPCHAIN_H
