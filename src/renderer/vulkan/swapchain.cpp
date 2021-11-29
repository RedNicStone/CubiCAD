//
// Created by nic on 09/03/2021.
//

#include "swapchain.h"

#include "imageview.h"


SwapChain::SwapChain(Device *pDevice,
                     Window *pWindow,
                     Queue *pQueue,
                     uint32_t imageCount_v,
                     std::vector<uint32_t> &accessingQueues) : device(pDevice), window(pWindow), presentQueue(pQueue) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice->getPhysicalDevice().getHandle(),
                                              pWindow->getSurface(),
                                              &capabilities);

    imageCount = std::max(capabilities.minImageCount, imageCount_v);
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat();

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = pWindow->getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = pWindow->getSurfaceExtend();
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = chooseSwapPresentMode();
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (accessingQueues.size() >= 2) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(accessingQueues.size());
        createInfo.pQueueFamilyIndices = accessingQueues.data();
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    if (vkCreateSwapchainKHR(pDevice->getHandle(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(pDevice->getHandle(), swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(pDevice->getHandle(), swapChain, &imageCount, swapChainImages.data());

    imageFormat = surfaceFormat.format;

    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = swapChainImages[i];
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = imageFormat;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(device->getHandle(), &viewCreateInfo, nullptr, &swapChainImageViews[i]);
    }

    createSyncObjects();

    listener.listen<PreRenderAcquireFrameEvent>([this](auto &&PH1) {
      acquireNextFrame(std::forward<decltype(PH1)>(PH1));
    });

    listener.listen<PostRenderPresentImageEvent>([this](auto &&PH1) {
      presentImage(std::forward<decltype(PH1)>(PH1));
    });
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
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->getPhysicalDevice().getHandle(),
                                         window->getSurface(),
                                         &formatCount,
                                         nullptr);

    std::vector<VkSurfaceFormatKHR> formats;
    if (formatCount != 0) {
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device->getPhysicalDevice().getHandle(),
                                             window->getSurface(),
                                             &formatCount,
                                             formats.data());
    }

    for (const auto &availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return formats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode() {
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->getPhysicalDevice().getHandle(),
                                              window->getSurface(),
                                              &presentModeCount,
                                              nullptr);

    std::vector<VkPresentModeKHR> presentModes;
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device->getPhysicalDevice().getHandle(),
                                                  window->getSurface(),
                                                  &presentModeCount,
                                                  presentModes.data());
    }

    for (const auto availablePresentMode : presentModes) {
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
    VkExtent2D surfaceExtend{};
    surfaceExtend = window->getSurfaceExtend();
    while (surfaceExtend.height == 0) {
        glfwWaitEvents();
        surfaceExtend = window->getSurfaceExtend();
    }

    vkDeviceWaitIdle(device->getHandle());

}

void SwapChain::acquireNextFrame(PreRenderAcquireFrameEvent event) {

    std::cout << "acquiring image #" << currentFrame << std::endl;

    if (event.device != device)
        return;

    inFlightFences[0].waitForSignal();
    inFlightFences[0].resetState();

    std::cout << "acquiring using image index: " << currentImageIndex << std::endl;
    std::cout << "acquiring using image: " << currentFrame << std::endl;
    std::cout << "acquiring using fence: " << inFlightFences[0].getHandle() << std::endl;
    std::cout << "acquiring using semaphore: " << imageAvailableSemaphores[0].getHandle() << std::endl;

    VkResult result = vkAcquireNextImageKHR(device->getHandle(),
                                            swapChain,
                                            UINT32_MAX,
                                            imageAvailableSemaphores[0].getHandle(),
                                            VK_NULL_HANDLE,
                                            &currentImageIndex);

    //if (imagesInFlight[currentImageIndex] != nullptr) {
    //    imagesInFlight[currentImageIndex]->waitForSignal();
    //}
    //imagesInFlight[currentImageIndex] = &inFlightFences[currentFrame];

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        window->setResized(true);
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("could not acquire image");
    }
}

void SwapChain::cleanupSwapChain() {

}

void SwapChain::createSyncObjects() {
    imageAvailableSemaphores = std::vector<Semaphore>();
    renderFinishedSemaphores = std::vector<Semaphore>();
    inFlightFences = std::vector<Fence>();
    imageAvailableSemaphores.reserve(imageCount);
    renderFinishedSemaphores.reserve(imageCount);
    inFlightFences.reserve(imageCount);
    for (uint32_t i = 0; i < imageCount; i++) {
        imageAvailableSemaphores.emplace_back(device);
        renderFinishedSemaphores.emplace_back(device);
        inFlightFences.emplace_back(device, true);
    }
    imagesInFlight = std::vector<Fence*>(imageCount, nullptr);
}

void SwapChain::presentImage(PostRenderPresentImageEvent event) {

    std::cout << "presenting image #" << currentFrame << std::endl;

    //inFlightFences[0].waitForSignal();

    std::cout << "acquiring using image index: " << currentImageIndex << std::endl;
    std::cout << "acquiring using image: " << currentFrame << std::endl;
    std::cout << "acquiring using semaphore: " << renderFinishedSemaphores[0].getHandle() << std::endl;

    if (event.device != device)
        return;

    VkSemaphore waitSemaphores[] = { renderFinishedSemaphores[0].getHandle() };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;

    VkSwapchainKHR swapchains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;

    presentInfo.pImageIndices = &currentImageIndex;

    VkResult result = vkQueuePresentKHR(presentQueue->getHandle(), &presentInfo);

    while (presentQueue->hasWorkSubmitted()) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
    }

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->getResized()) {
        window->resetResizeFence();
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % imageCount;
}

void SwapChain::doRenderLoop(std::vector<CommandBuffer>* comandBuffers, Queue * queue) {
    std::cout << "acquiring image #" << currentFrame << std::endl;

    inFlightFences[currentFrame].waitForSignal();

    currentImageIndex = 0;

    std::cout << "acquiring using image index: " << currentImageIndex << std::endl;
    std::cout << "acquiring using image: " << currentFrame << std::endl;
    std::cout << "acquiring using semaphore: " << imageAvailableSemaphores[currentFrame].getHandle() << std::endl;

    VkResult result = vkAcquireNextImageKHR(device->getHandle(),
                                            swapChain,
                                            UINT32_MAX,
                                            imageAvailableSemaphores[currentFrame].getHandle(),
                                            VK_NULL_HANDLE,
                                            &currentImageIndex);

    //if (imagesInFlight[currentImageIndex] != nullptr) {
    //    imagesInFlight[currentImageIndex]->waitForSignal();
    //}
    //imagesInFlight[currentImageIndex] = &inFlightFences[currentFrame];

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        window->setResized(true);
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("could not acquire image");
    }

    //swapChain.getPresentFence()->waitForSignal();
    //swapChain.getPresentFence()->resetState();

    if (imagesInFlight[currentImageIndex] != nullptr) {
        imagesInFlight[currentImageIndex]->waitForSignal();
    }

    imagesInFlight[currentImageIndex] = &inFlightFences[currentFrame];

    std::cout << "submitting using image index: " << currentImageIndex << std::endl;
    std::cout << "submitting using image: " << currentFrame << std::endl;
    std::cout << "submitting using signal semaphore: " << renderFinishedSemaphores[currentFrame].getHandle() <<
    std::endl;
    std::cout << "submitting using wait semaphore: " << imageAvailableSemaphores[currentFrame].getHandle() << std::endl;
    std::vector<Semaphore*> signalSemaphores = {&renderFinishedSemaphores[currentFrame]};
    std::vector<Semaphore*> waitSemaphores = { &imageAvailableSemaphores[currentFrame] };

    inFlightFences[currentFrame].resetState();

    comandBuffers->at(currentImageIndex).submitToQueue(signalSemaphores,
                                                                    { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
                                                       waitSemaphores,
                                                   queue,
                                                                    &inFlightFences[0]);


    std::cout << "presenting image #" << currentFrame << std::endl;

    //inFlightFences[0].waitForSignal();

    std::cout << "acquiring using image index: " << currentImageIndex << std::endl;
    std::cout << "acquiring using image: " << currentFrame << std::endl;
    std::cout << "acquiring using semaphore: " << renderFinishedSemaphores[currentFrame].getHandle() << std::endl;

    VkSemaphore waitSemaphoress[] = { renderFinishedSemaphores[currentFrame].getHandle() };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphoress;

    VkSwapchainKHR swapchains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;

    presentInfo.pImageIndices = &currentImageIndex;

    result = vkQueuePresentKHR(presentQueue->getHandle(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->getResized()) {
        window->resetResizeFence();
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % imageCount;
}
