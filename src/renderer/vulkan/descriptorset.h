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


template<class type>
class UniformBuffer;

class Buffer;

class DescriptorSet : public VulkanClass<VkDescriptorSet> {
  private:
    std::shared_ptr<Device> device;
    std::shared_ptr<DescriptorSetLayout> setLayout;
    std::shared_ptr<DescriptorPool> pool;

  public:
    static std::shared_ptr<DescriptorSet> create(std::shared_ptr<DescriptorSetLayout> pLayout,
                                                 const std::shared_ptr<DescriptorPool> &pPool);

    static std::shared_ptr<DescriptorSet> create(std::shared_ptr<DescriptorSetLayout> pLayout,
                                                 const std::shared_ptr<DescriptorPool> &pPool,
                                                 VkResult& result);

    static std::vector<std::shared_ptr<DescriptorSet>> create(std::vector<std::shared_ptr<DescriptorSetLayout>> pLayout,
                                                 const std::shared_ptr<DescriptorPool> &pPool);

    template<class type>
    void updateUniformBuffer(std::shared_ptr<UniformBuffer<type>> buffer,
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

    template<class ...type>
    void updateUniformBuffer(std::vector<std::shared_ptr<UniformBuffer<type...>>> buffer,
                             std::vector<uint32_t> binding,
                             std::vector<VkDeviceSize> offset = {},
                             std::vector<uint32_t> arrayElement = {}) {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets(buffer.size());

        if (offset.empty()) {
            offset = std::vector<VkDeviceSize>(buffer.size(), 0);
        }

        if (arrayElement.empty()) {
            offset = std::vector<VkDeviceSize>(buffer.size(), 0);
        }

        for (uint32_t i = 0; i < buffer.size(); i++) {
            VkDescriptorBufferInfo info{};
            info.buffer = buffer->getBuffer()->getHandle();
            info.range = buffer[i]->getSize();
            info.offset = offset[i];

            writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[i].dstSet = handle;
            writeDescriptorSets[i].dstBinding = binding[i];
            writeDescriptorSets[i].dstArrayElement = arrayElement[i];
            writeDescriptorSets[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSets[i].descriptorCount = 1;
            writeDescriptorSets[i].pBufferInfo = &info;
            writeDescriptorSets[i].pImageInfo = nullptr;
            writeDescriptorSets[i].pTexelBufferView = nullptr;
        }

        vkUpdateDescriptorSets(device->getHandle(), buffer.size(), writeDescriptorSets.data(), 0, nullptr);
    }

    //void updateCombinedImageSampler()  // todo implement samplers
    void updateStorageBuffer(const std::shared_ptr<Buffer> &buffer,
                             uint32_t binding,
                             uint32_t arrayElement = 0,
                             VkDeviceSize offset = 0,
                             VkDeviceSize range = VK_WHOLE_SIZE);

    void updateImage(const std::shared_ptr<ImageView> &imageView, uint32_t binding, uint32_t arrayElement = 0);
    void updateImage(const std::shared_ptr<ImageView> &imageView, VkDescriptorType type, uint32_t binding, uint32_t
    arrayElement = 0);

    std::shared_ptr<Device> getDevice() { return device; }

    std::shared_ptr<DescriptorPool> getPool() { return pool; }

    std::shared_ptr<DescriptorSetLayout> getLayout() { return setLayout; }

    ~DescriptorSet();
};

#endif //CUBICAD_DESCRIPTORSET_H
