//
// Created by nic on 23/08/2021.
//

#include "descriptorset.h"

#include <utility>


std::shared_ptr<DescriptorSet> DescriptorSet::create(std::shared_ptr<DescriptorSetLayout> pLayout,
                                                     const std::shared_ptr<DescriptorPool> &pPool) {
    auto descriptorSet = std::make_shared<DescriptorSet>();
    descriptorSet->device = pPool->getDevice();
    descriptorSet->pool = pPool;
    descriptorSet->setLayout = std::move(pLayout);

    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = pPool->getHandle();
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = descriptorSet->setLayout->getHandlePtr();

    if (vkAllocateDescriptorSets(descriptorSet->device->getHandle(), &allocateInfo, &descriptorSet->handle) !=
    VK_SUCCESS) {
        throw std::runtime_error("could not create descriptor set!");
    }

    return descriptorSet;
}

std::shared_ptr<DescriptorSet> DescriptorSet::create(std::shared_ptr<DescriptorSetLayout> pLayout,
                                                     const std::shared_ptr<DescriptorPool> &pPool,
                                                     VkResult& result) {
    auto descriptorSet = std::make_shared<DescriptorSet>();
    descriptorSet->device = pPool->getDevice();
    descriptorSet->pool = pPool;
    descriptorSet->setLayout = std::move(pLayout);

    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = pPool->getHandle();
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = descriptorSet->setLayout->getHandlePtr();

    result = vkAllocateDescriptorSets(descriptorSet->device->getHandle(), &allocateInfo, &descriptorSet->handle);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("could not create descriptor set!");
    }

    return descriptorSet;
}

std::vector<std::shared_ptr<DescriptorSet>> DescriptorSet::create(std::vector<std::shared_ptr<DescriptorSetLayout>> pLayout,
                                                                  const std::shared_ptr<DescriptorPool> &pPool) {
    auto descriptorSets = std::vector<std::shared_ptr<DescriptorSet>>(pLayout.size(), std::make_shared<DescriptorSet>
        ());
    auto setLayouts = std::vector<VkDescriptorSetLayout>(pLayout.size());

    for (uint32_t i = 0; i < pLayout.size(); i++) {
        setLayouts[i] = pLayout[i]->getHandle();
    }

    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = pPool->getHandle();
    allocateInfo.descriptorSetCount = static_cast<uint32_t>(pLayout.size());
    allocateInfo.pSetLayouts = setLayouts.data();

    auto descriptorSetHandles = std::vector<VkDescriptorSet>(pLayout.size());

    if (vkAllocateDescriptorSets(pPool->getDevice()->getHandle(), &allocateInfo, descriptorSetHandles.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("could not create descriptor set!");
    }

    for (uint32_t i = 0; i < pLayout.size(); i++) {
        descriptorSets[i]->device = pPool->getDevice();
        descriptorSets[i]->pool = pPool;
        descriptorSets[i]->setLayout = pLayout[i];

        descriptorSets[i]->handle = descriptorSetHandles[i];
    }

    return descriptorSets;
}

void DescriptorSet::updateStorageBuffer(const std::shared_ptr<Buffer> &buffer,
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

void DescriptorSet::updateImage(const std::shared_ptr<ImageView> &imageView,
                                       uint32_t binding,
                                       uint32_t arrayElement) {
    updateImage(imageView, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, binding, arrayElement);
}

void DescriptorSet::updateImage(const std::shared_ptr<ImageView> &imageView,
                                VkDescriptorType type,
                                uint32_t binding,
                                uint32_t arrayElement) {
    VkDescriptorImageInfo info = {};
    info.imageView = imageView->getHandle();
    info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = handle;
    write.dstBinding = binding;
    write.dstArrayElement = arrayElement;
    write.descriptorType = type;
    write.descriptorCount = 1;
    write.pImageInfo = &info;

    vkUpdateDescriptorSets(device->getHandle(), 1, &write, 0, nullptr);
}

DescriptorSet::~DescriptorSet() {
    vkFreeDescriptorSets(device->getHandle(), pool->getHandle(), 1, &handle);
}
