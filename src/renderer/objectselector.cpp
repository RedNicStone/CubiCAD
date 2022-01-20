//
// Created by nic on 20/01/2022.
//

#include "objectselector.h"


std::shared_ptr<ObjectSelector> ObjectSelector::create(const std::shared_ptr<Device>& device,
                                                       const std::shared_ptr<Queue>& renderQueue,
                                                       const std::shared_ptr<Queue>& transferQueue,
                                                       VkExtent2D extent) {
    auto objectSelector = std::make_shared<ObjectSelector>();

    std::vector<uint32_t> accessingQueues{ renderQueue->getQueueFamilyIndex(),
                                           transferQueue->getQueueFamilyIndex() };

    auto end = accessingQueues.end();
    for (auto it = accessingQueues.begin(); it != end; ++it) {
        end = std::remove(it + 1, end, *it);
    }
    accessingQueues.erase(end, accessingQueues.end());

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.extent = { extent.width, extent.height, 0};
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_R32_UINT;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    objectSelector->image = Image::create(device, VMA_MEMORY_USAGE_GPU_TO_CPU, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, imageCreateInfo, accessingQueues);

    objectSelector->imageView = ImageView::create(objectSelector->image, VK_IMAGE_VIEW_TYPE_2D,
                                                  VK_IMAGE_ASPECT_COLOR_BIT);
}
