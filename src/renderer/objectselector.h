//
// Created by nic on 20/01/2022.
//

#ifndef CUBICAD_OBJECTSELECTOR_H
#define CUBICAD_OBJECTSELECTOR_H

#include "../renderer/vulkan/imageview.h"


class ObjectSelector {
  private:
    std::shared_ptr<Image> image;
    std::shared_ptr<ImageView> imageView;

  public:
    static std::shared_ptr<ObjectSelector> create(const std::shared_ptr<Device>& device,
                                           const std::shared_ptr<Queue>& renderQueue,
                                           const std::shared_ptr<Queue>& transferQueue,
                                           VkExtent2D extent);

    std::shared_ptr<Image> getImage() { return image; }
    std::shared_ptr<ImageView> getImageView() { return imageView; }
};

#endif //CUBICAD_OBJECTSELECTOR_H
