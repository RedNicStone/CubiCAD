//
// Created by nic on 23/08/2021.
//

#pragma once

#ifndef CUBICAD_DESCRIPTORSET_H
#define CUBICAD_DESCRIPTORSET_H

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "vulkanclass.h"
#include "device.h"
#include "descriptorlayout.h"
#include "descriptorpool.h"
#include "imageview.h"
#include "uniformbuffer.h"


class DescriptorSet : public VulkanClass<VkDescriptorSet> {
  private:
    Device *device;
    DescriptorSetLayout *setLayout;
    DescriptorPool *pool;

  public:
    DescriptorSet(DescriptorSetLayout *pLayout, DescriptorPool *pPool);

    template<class type>
    void updateUniformBuffer(UniformBuffer<type> *buffer,
                                            uint32_t binding,
                                            VkDeviceSize offset = 0,
                                            uint32_t arrayElement = 0) {
        VkDescriptorBufferInfo info{};
        info.buffer = buffer->getBuffer()->getHandle();
        info.range = sizeof(type);
        info.offset = offset;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = handle;
        write.dstBinding = binding;
        write.dstArrayElement = arrayElement;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &info;
        write.pImageInfo = nullptr;
        write.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(device->getHandle(), 1, &write, 0, nullptr);
    }
    //void updateCombinedImageSampler()  // todo implement samplers
    void updateStorageBuffer(Buffer *buffer,
                             uint32_t binding,
                             uint32_t arrayElement = 0,
                             VkDeviceSize offset = 0,
                             VkDeviceSize range = VK_WHOLE_SIZE);
    void updateStorageImage(ImageView *imageView, uint32_t binding, uint32_t arrayElement = 0);

    Device *getDevice() { return device; }

    DescriptorPool *getPool() { return pool; }

    DescriptorSetLayout *getLayout() { return setLayout; }

    ~DescriptorSet();
};

#endif //CUBICAD_DESCRIPTORSET_H
