//
// Created by nic on 20/04/2021.
//

#pragma once

#ifndef CUBICAD_IMAGEVIEW_H
#define CUBICAD_IMAGEVIEW_H

#include <vulkan/vulkan.h>

#include "image.h"

#include <iostream>


class Image;

class ImageView : public VulkanClass<VkImageView> {
  private:
    std::shared_ptr<Image> image;

  public:
    static std::shared_ptr<ImageView> create(std::shared_ptr<Image> pImage,
                                             VkImageViewType viewType,
                                             VkImageSubresourceRange &subresourceRange);
    static std::shared_ptr<ImageView> create(std::shared_ptr<Image> pImage,
                                             VkImageViewType viewType,
                                             VkImageAspectFlags aspectFlags);

    std::shared_ptr<Image> getImage() { return image; }

    ~ImageView();
};

#endif //CUBICAD_IMAGEVIEW_H
