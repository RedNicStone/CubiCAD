//
// Created by nic on 20/01/2022.
//

#ifndef CUBICAD_FRAMEBUFFERSELECTOR_H
#define CUBICAD_FRAMEBUFFERSELECTOR_H

#include "../renderer/vulkan/imageview.h"


class FramebufferSelector {
  private:
    std::shared_ptr<Image> image;
    std::shared_ptr<ImageView> imageView;

    uint32_t* data;

  public:
    static std::shared_ptr<FramebufferSelector> create(const std::shared_ptr<Device>& device,
                                                       const std::shared_ptr<Queue>& renderQueue,
                                                       VkExtent2D extent);

    std::shared_ptr<Image> getImage() { return image; }
    std::shared_ptr<ImageView> getImageView() { return imageView; }

    void* getImageData() { return data; }
    uint32_t getIDAtPosition(VkExtent2D position);

    ~FramebufferSelector();
};

#endif //CUBICAD_FRAMEBUFFERSELECTOR_H
