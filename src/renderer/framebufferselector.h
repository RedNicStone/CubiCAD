//
// Created by nic on 20/01/2022.
//

#ifndef CUBICAD_FRAMEBUFFERSELECTOR_H
#define CUBICAD_FRAMEBUFFERSELECTOR_H

#include <fstream>

#include "../renderer/vulkan/imageview.h"


/// Class that handles back-buffering for selecting objects with the mouse
class FramebufferSelector {
  private:
    std::shared_ptr<Image> image;

    VkExtent2D extent;
    uint32_t *data;
    VkSubresourceLayout imageLayout;

  public:
    /// Create a new framebuffer selector
    /// \param device Device the selector should be created on
    /// \param renderQueue Render queue the selector is used in
    /// \param extent Render extend of the buffer
    /// \return Valid handle to the FramebufferSelector
    static std::shared_ptr<FramebufferSelector> create(const std::shared_ptr<Device> &device,
                                                       const std::shared_ptr<Queue> &renderQueue,
                                                       VkExtent2D extent);

    std::shared_ptr<Image> getImage() { return image; }

    /// Transfer the buffer into a CPU readable one
    /// \param commandBuffer Command buffer to write commands to
    void transferLayoutRead(const std::shared_ptr<CommandBuffer> &commandBuffer);
    /// Transfer the buffer into a GPU writeable one
    /// \param commandBuffer Command buffer to write commands to
    void transferLayoutWrite(const std::shared_ptr<CommandBuffer> &commandBuffer);

    /// Get CPU handle to the buffer data
    /// \return Void handle to buffer data
    void *getImageData() { return data; }

    /// Get object ID of object at pixel position
    /// \param position Pixel position
    /// \return ID of the object
    uint32_t getIDAtPosition(VkExtent2D position);

    ~FramebufferSelector();
};

#endif //CUBICAD_FRAMEBUFFERSELECTOR_H
