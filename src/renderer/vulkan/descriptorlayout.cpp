//
// Created by nic on 22/05/2021.
//

#include "descriptorlayout.h"


DescriptorSetLayout::DescriptorSetLayout(Device *device, std::vector<VkDescriptorSetLayoutBinding> &bindings_) : device(
    device) {
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings_.size());
    layoutInfo.pBindings = bindings_.data();

    if (vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance descriptor set layout!");
    }
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device->getHandle(), handle, nullptr);
}
