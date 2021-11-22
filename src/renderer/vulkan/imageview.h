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
    Image *image;

  public:
    ImageView(Image *pImage, VkImageViewType viewType, VkImageSubresourceRange &subresourceRange);
    ImageView(Image *pImage, VkImageViewType viewType, VkImageAspectFlags aspectFlags);

    Image &getImage() { return *image; }

    ~ImageView();
};

#endif //CUBICAD_IMAGEVIEW_H
