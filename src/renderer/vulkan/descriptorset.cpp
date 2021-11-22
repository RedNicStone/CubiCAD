//
// Created by nic on 23/08/2021.
//

#include "descriptorset.h"


DescriptorSet::DescriptorSet(DescriptorSetLayout *pLayout, DescriptorPool *pPool)
    : device(pPool->getDevice()), setLayout(pLayout), pool(pPool) {
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = pPool->getHandle();
    allocateInfo.descriptorSetCount = 1;
    VkDescriptorSetLayout layout = setLayout->getHandle();
    allocateInfo.pSetLayouts = &layout;

    vkAllocateDescriptorSets(device->getHandle(), &allocateInfo, &handle);
}

void DescriptorSet::updateStorageBuffer(Buffer *buffer,
                                        uint32_t binding,
                                        uint32_t arrayElement,
                                        VkDeviceSize offset,
                                        VkDeviceSize range) {
    VkDescriptorBufferInfo info = {};
    info.buffer = buffer->getHandle();
    info.range = range;
    info.offset = offset;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = handle;
    write.dstBinding = binding;
    write.dstArrayElement = arrayElement;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &info;

    vkUpdateDescriptorSets(device->getHandle(), 1, &write, 0, nullptr);
}

void DescriptorSet::updateStorageImage(ImageView *imageView, uint32_t binding, uint32_t arrayElement) {
    VkDescriptorImageInfo info = {};
    info.imageView = imageView->getHandle();
    info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = handle;
    write.dstBinding = binding;
    write.dstArrayElement = arrayElement;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write.descriptorCount = 1;
    write.pImageInfo = &info;

    vkUpdateDescriptorSets(device->getHandle(), 1, &write, 0, nullptr);
}

DescriptorSet::~DescriptorSet() {
    vkFreeDescriptorSets(device->getHandle(), pool->getHandle(), 1, &handle);
}
