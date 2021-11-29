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

    Queue *presentQueue;
    uint32_t imageCount;
    VkFormat imageFormat;
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    uint32_t currentFrame{0};
    uint32_t currentImageIndex{0};
    std::vector<Semaphore> imageAvailableSemaphores;
    std::vector<Semaphore> renderFinishedSemaphores;
    std::vector<Fence> inFlightFences;
    std::vector<Fence*> imagesInFlight;

    dexode::EventBus::Listener listener{publicRenderBus.getBus()};

    Device *device;
    Window *window;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat();
    VkPresentModeKHR chooseSwapPresentMode();

    void createSyncObjects();
    void cleanupSwapChain();

    void recreateSwapChain();
    void acquireNextFrame(PreRenderAcquireFrameEvent event);
    void presentImage(PostRenderPresentImageEvent event);

  public:
    //construction
    SwapChain(Device *pDevice,
              Window *pWindow,
              Queue *pQueue,
              uint32_t imageCount_v,
              std::vector<uint32_t> &accessingQueues);

    VkSurfaceCapabilitiesKHR querySurfaceCapabilities(VkPhysicalDevice *physicalDevice);
    std::vector<VkSurfaceFormatKHR> querySurfaceFormats(VkPhysicalDevice *physicalDevice);
    std::vector<VkPresentModeKHR> queryPresentModes(VkPhysicalDevice *physicalDevice);

    VkFormat getFormat() { return imageFormat; }

    VkImage getSwapChainImage(uint32_t i) { return swapChainImages[i]; }
    VkImageView getSwapChainImageView(uint32_t i) { return swapChainImageViews[i]; }

    VkExtent2D getSwapExtent();

    [[nodiscard]] uint32_t getImageCount() const { return imageCount; }
    [[nodiscard]] uint32_t getCurrentFrame() const { return currentFrame; }
    [[nodiscard]] uint32_t getCurrentImageIndex() const { return currentImageIndex; }

    std::vector<Semaphore*> getRenderSignalSemaphores() { return { &renderFinishedSemaphores[0] }; }
    std::vector<Semaphore*> getRenderWaitSemaphores() { return { &imageAvailableSemaphores[0] }; }
    Fence* getPresentFence() { return &inFlightFences[0]; }

    void doRenderLoop(std::vector<CommandBuffer>* comandBuffers, Queue * queue);

    ~SwapChain();
};

#endif //CUBICAD_SWAPCHAIN_H
