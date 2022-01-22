//
// Created by nic on 20/01/2022.
//

#ifndef CUBICAD_FRAMEBUFFERSELECTOR_H
#define CUBICAD_FRAMEBUFFERSELECTOR_H

#include <fstream>

#include "../renderer/vulkan/imageview.h"


class FramebufferSelector {
  private:
    std::shared_ptr<Image> image;

    VkExtent2D extent;
    uint32_t* data;
    VkSubresourceLayout imageLayout;

  public:
    static std::shared_ptr<FramebufferSelector> create(const std::shared_ptr<Device>& device,
                                                       const std::shared_ptr<Queue>& renderQueue,
                                                       VkExtent2D extent);

    std::shared_ptr<Image> getImage() { return image; }

    void transferLayoutRead(const std::shared_ptr<CommandBuffer>& commandBuffer);
    void transferLayoutWrite(const std::shared_ptr<CommandBuffer>& commandBuffer);

    void* getImageData() { return data; }
    uint32_t getIDAtPosition(VkExtent2D position);

    ~FramebufferSelector();
};

#endif //CUBICAD_FRAMEBUFFERSELECTOR_H
