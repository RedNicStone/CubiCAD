//
// Created by nic on 09/03/2021.
//

#include "swapchain.h"

#include <utility>

#include "imageview.h"


std::shared_ptr<SwapChain> SwapChain::create(const std::shared_ptr<Device> &pDevice,
                                             const std::shared_ptr<Window> &pWindow,
                                             std::shared_ptr<Queue> pQueue,
                                             uint32_t imageCount_v,
                                             std::vector<uint32_t> &accessingQueues) {
    auto swapChain = std::make_shared<SwapChain>();
    swapChain->device = pDevice;
    swapChain->window = pWindow;
    swapChain->presentQueue = std::move(pQueue);

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice->getPhysicalDevice()->getHandle(),
                                              pWindow->getSurface(),
                                              &swapChain->capabilities);

    swapChain->imageCount = std::max(swapChain->capabilities.minImageCount, imageCount_v);
    if (swapChain->capabilities.maxImageCount > 0 && swapChain->imageCount > swapChain->capabilities.maxImageCount) {
        swapChain->imageCount = swapChain->capabilities.maxImageCount;
    }

    VkSurfaceFormatKHR surfaceFormat = swapChain->chooseSwapSurfaceFormat();

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = pWindow->getSurface();
    createInfo.minImageCount = swapChain->imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = pWindow->getSurfaceExtend();
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChain->capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = swapChain->chooseSwapPresentMode();
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
    createInfo.pQueueFamilyIndices = accessingQueues.data();

    if (accessingQueues.size() >= 2) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (vkCreateSwapchainKHR(pDevice->getHandle(), &createInfo, nullptr, &swapChain->swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(pDevice->getHandle(), swapChain->swapChain, &swapChain->imageCount, nullptr);
    swapChain->swapChainImages.resize(swapChain->imageCount);
    vkGetSwapchainImagesKHR(pDevice->getHandle(),
                            swapChain->swapChain,
                            &swapChain->imageCount,
                            swapChain->swapChainImages.data());

    swapChain->imageFormat = surfaceFormat.format;

    swapChain->swapChainImageViews.resize(swapChain->swapChainImages.size());

    for (size_t i = 0; i < swapChain->swapChainImages.size(); i++) {
        VkImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = swapChain->swapChainImages[i];
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = swapChain->imageFormat;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(swapChain->device->getHandle(), &viewCreateInfo, nullptr, &swapChain->swapChainImageViews[i]);
    }

    swapChain->createSyncObjects();

    swapChain->listener.listen<PreRenderAcquireFrameEvent>([&swapChain](auto &&PH1) {
      swapChain->acquireNextFrame(std::forward<decltype(PH1)>(PH1));
    });

    swapChain->listener.listen<PostRenderPresentImageEvent>([&swapChain](auto &&PH1) {
      swapChain->presentImage(std::forward<decltype(PH1)>(PH1));
    });

    return swapChain;
}

VkSurfaceCapabilitiesKHR SwapChain::querySurfaceCapabilities(VkPhysicalDevice *physicalDevice) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physicalDevice, window->getSurface(), &surfaceCapabilities);

    return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR> SwapChain::querySurfaceFormats(VkPhysicalDevice *physicalDevice) {
    std::vector<VkSurfaceFormatKHR> surfaceFormats;

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, window->getSurface(), &formatCount, nullptr);

    if (formatCount != 0) {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice,
                                             window->getSurface(),
                                             &formatCount,
                                             surfaceFormats.data());
    }

    return surfaceFormats;
}

std::vector<VkPresentModeKHR> SwapChain::queryPresentModes(VkPhysicalDevice *physicalDevice) {
    std::vector<VkPresentModeKHR> presentModes;

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, window->getSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice,
                                                  window->getSurface(),
                                                  &presentModeCount,
                                                  presentModes.data());
    }

    return presentModes;
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat() {
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->getPhysicalDevice()->getHandle(),
                                         window->getSurface(),
                                         &formatCount,
                                         nullptr);

    std::vector<VkSurfaceFormatKHR> formats;
    if (formatCount != 0) {
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device->getPhysicalDevice()->getHandle(),
                                             window->getSurface(),
                                             &formatCount,
                                             formats.data());
    }

    for (const auto &availableFormat: formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return formats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode() {
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->getPhysicalDevice()->getHandle(),
                                              window->getSurface(),
                                              &presentModeCount,
                                              nullptr);

    std::vector<VkPresentModeKHR> presentModes;
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device->getPhysicalDevice()->getHandle(),
                                                  window->getSurface(),
                                                  &presentModeCount,
                                                  presentModes.data());
    }

    for (const auto availablePresentMode: presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::getSwapExtent() {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window->getWindow(), &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::max(capabilities.minImageExtent.width,
                     std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height =
            std::max(capabilities.minImageExtent.height,
                     std::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }
}

SwapChain::~SwapChain() {
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        vkDestroyImageView(device->getHandle(), swapChainImageViews[i], nullptr);
        //vkDestroyImage(device->getHandle(), swapChainImages[i], nullptr);
    }

    vkDestroySwapchainKHR(device->getHandle(), swapChain, nullptr);
}

void SwapChain::recreateSwapChain() {
    VkExtent2D surfaceExtend;
    surfaceExtend = window->getSurfaceExtend();
    while (surfaceExtend.height == 0) {
        glfwWaitEvents();
        surfaceExtend = window->getSurfaceExtend();
    }

    vkDeviceWaitIdle(device->getHandle());

}

void SwapChain::acquireNextFrame(const PreRenderAcquireFrameEvent &event) {
    if (event.device != device)
        return;

    inFlightFences[currentFrame]->waitForSignal();

    VkResult
        result =
        vkAcquireNextImageKHR(device->getHandle(),
                              swapChain,
                              UINT32_MAX,
                              imageAvailableSemaphores[currentFrame]->getHandle(),
                              VK_NULL_HANDLE,
                              &currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        window->setResized(true);
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("could not acquire image");
    }

    if (imagesInFlight[currentImageIndex] != nullptr) {
        imagesInFlight[currentImageIndex]->waitForSignal();
    }
    imagesInFlight[currentImageIndex] = inFlightFences[currentFrame];

    inFlightFences[currentFrame]->resetState();
}

void SwapChain::cleanupSwapChain() {

}

void SwapChain::createSyncObjects() {
    imageAvailableSemaphores = std::vector<std::shared_ptr<Semaphore>>();
    renderFinishedSemaphores = std::vector<std::shared_ptr<Semaphore>>();
    inFlightFences = std::vector<std::shared_ptr<Fence>>();
    imageAvailableSemaphores.reserve(imageCount);
    renderFinishedSemaphores.reserve(imageCount);
    inFlightFences.reserve(imageCount);
    for (uint32_t i = 0; i < imageCount; i++) {
        imageAvailableSemaphores.push_back(Semaphore::create(device));
        renderFinishedSemaphores.push_back(Semaphore::create(device));
        inFlightFences.push_back(Fence::create(device, true));
    }
    imagesInFlight = std::vector<std::shared_ptr<Fence>>(imageCount, nullptr);
}

void SwapChain::presentImage(const PostRenderPresentImageEvent &event) {
    if (event.device != device)
        return;

    VkSemaphore waitSemaphores[] = {renderFinishedSemaphores[currentFrame]->getHandle()};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;

    VkSwapchainKHR swapchains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;

    presentInfo.pImageIndices = &currentImageIndex;

    VkResult result = vkQueuePresentKHR(presentQueue->getHandle(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->getResized()) {
        window->resetResizeFence();
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % imageCount;
}
