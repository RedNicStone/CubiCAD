//
// Created by nic on 28/01/2022.
//

#pragma once

#ifndef CUBICAD_RENDERMANAGER_H
#define CUBICAD_RENDERMANAGER_H

#include "framebufferselector.h"
#include "camera.h"
#include "materiallibrary.h"
#include "texturelibrary.h"
#include "meshlibrary.h"


class RenderManager {
  private:
    std::shared_ptr<PhysicalDevice> physicalDevice;
    std::shared_ptr<Device> device;

    std::shared_ptr<Queue> renderQueue;
    std::shared_ptr<Queue> transferQueue;
    std::shared_ptr<Queue> presentQueue;

    std::shared_ptr<SwapChain> swapChain;
    std::shared_ptr<FrameBuffer> frameBuffer;
    VkExtent2D swapChainExtent;

    std::shared_ptr<Image> depthImage;
    std::shared_ptr<ImageView> depthImageView;

    std::shared_ptr<RenderPass> renderPass;
    uint32_t renderSubpass;
    uint32_t UISubpass;

    std::shared_ptr<FramebufferSelector> objectBuffer;
    std::shared_ptr<Image> objectBufferImage;
    std::shared_ptr<ImageView> objectBufferImageView;
    std::shared_ptr<CommandPool> renderPool;

    std::shared_ptr<Camera> camera;
    std::shared_ptr<UIRenderer> UIRenderer;

    std::shared_ptr<CommandPool> transferPool;
    std::shared_ptr<CommandPool> presentPool;

    std::shared_ptr<Scene> currentScene;

    std::shared_ptr<MaterialLibrary> materialLibrary;
    std::shared_ptr<TextureLibrary> textureLibrary;
    std::shared_ptr<MeshLibrary> meshLibrary;

  public:
    std::shared_ptr<RenderManager> create();
};

#endif //CUBICAD_RENDERMANAGER_H
