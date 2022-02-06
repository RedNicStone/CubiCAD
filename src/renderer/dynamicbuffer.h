//
// Created by nic on 13/01/2022.
//

#ifndef CUBICAD_DYNAMICBUFFER_H
#define CUBICAD_DYNAMICBUFFER_H

#include <memory>
#include <vector>

#include "vulkan/buffer.h"


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
    static std::shared_ptr<DynamicBuffer> create(const std::shared_ptr<Device> &pDevice,
                                                 const std::vector<uint32_t> &vAccessingQueues,
                                                 VmaMemoryUsage vMemoryUsage,
                                                 VkBufferUsageFlags vBufferUsage,
                                                 VkMemoryPropertyFlags vPreferredFlags = 0,
                                                 VkMemoryPropertyFlags vRequiredFlags = 0);

    std::shared_ptr<Buffer> getBuffer(VkDeviceSize size = 0);
    std::shared_ptr<Buffer> getBufferPreserveContents(VkDeviceSize size,
                                                      const std::shared_ptr<CommandPool> &commandPool);

    void **map();
    void unmap();
};

#endif //CUBICAD_DYNAMICBUFFER_H
