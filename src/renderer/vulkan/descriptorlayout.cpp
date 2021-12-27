//
// Created by nic on 22/05/2021.
//

#include "descriptorlayout.h"

#include <utility>


std::shared_ptr<DescriptorSetLayout> DescriptorSetLayout::create(std::shared_ptr<Device> pDevice,
                                                                 std::vector<VkDescriptorSetLayoutBinding> &bindings) {
    auto descriptorSetLayout = std::make_shared<DescriptorSetLayout>();
    descriptorSetLayout->device = std::move(pDevice);

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(descriptorSetLayout->device->getHandle(),
                                    &layoutInfo,
                                    nullptr,
                                    &descriptorSetLayout->handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to createInstance descriptor set layout!");
    }

    return descriptorSetLayout;
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device->getHandle(), handle, nullptr);
}
