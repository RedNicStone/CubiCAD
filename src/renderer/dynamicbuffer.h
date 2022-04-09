//
// Created by nic on 13/01/2022.
//

#ifndef CUBICAD_DYNAMICBUFFER_H
#define CUBICAD_DYNAMICBUFFER_H

#include <memory>
#include <vector>

#include "vulkan/buffer.h"


/// Class that is used to maintain dynamic buffers that are resized automatically
class DynamicBuffer {
  private:
    std::shared_ptr<Device> device;
    std::shared_ptr<Buffer> buffer = nullptr;

    std::vector<uint32_t> accessingQueues;
    VmaMemoryUsage memoryUsage;
    VkBufferUsageFlags bufferUsage;
    VkMemoryPropertyFlags preferredFlags;
    VkMemoryPropertyFlags requiredFlags;

    bool mapped = false;
    void *data;

  public:
    /// Create a new dynamic buffer
    /// \param pDevice Device to create the buffer on
    /// \param vAccessingQueues Vector of queue that will access this buffer
    /// \param vMemoryUsage Where the buffer will be used
    /// \param vBufferUsage What this buffer will be used for
    /// \param vPreferredFlags Flags that will be preferred during buffer creation
    /// \param vRequiredFlags Flags that are required during buffer creation
    /// \return Valid handle to the newly created buffer
    static std::shared_ptr<DynamicBuffer> create(const std::shared_ptr<Device> &pDevice,
                                                 const std::vector<uint32_t> &vAccessingQueues,
                                                 VmaMemoryUsage vMemoryUsage,
                                                 VkBufferUsageFlags vBufferUsage,
                                                 VkMemoryPropertyFlags vPreferredFlags = 0,
                                                 VkMemoryPropertyFlags vRequiredFlags = 0);

    /// Get the active buffer handle
    /// \param size Minimum required size. If the buffer is smaller than this it will be resized
    /// \return Active buffer handle
    std::shared_ptr<Buffer> getBuffer(VkDeviceSize size = 0);
    /// Get the active buffer handle but preserving the current buffer contents
    /// \param size Minimum required size. If the buffer is smaller than this it will be resized
    /// \return Active buffer handle
    std::shared_ptr<Buffer> getBufferPreserveContents(VkDeviceSize size,
                                                      const std::shared_ptr<CommandPool> &commandPool);

    /// Map the buffer to the CPU and get a writable handle (If allowed)
    /// \return CPU writable handle
    void **map();
    /// Unmap the buffer from the CPU
    void unmap();
};

#endif //CUBICAD_DYNAMICBUFFER_H
