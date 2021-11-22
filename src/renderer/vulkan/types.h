//
// Created by nic on 08/03/2021.
//

#ifndef CUBICAD_TYPES_H
#define CUBICAD_TYPES_H

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>


template<class T>
struct optional {
  private:
    T data;
    bool valid;

  public:
    optional() {
        valid = false;
    }

    explicit optional(T tData) {
        valid = true;
        data = tData;
    }

    bool has_value() const { return valid; }

    T &value() { return data; }

    optional<T> &operator=(T &tData) {
        data = tData;
        valid = true;
        return *this;
    }
};

struct QueueFamilyIndices {
    optional<uint32_t> graphicsFamily;
    optional<uint32_t> presentFamily;
    optional<uint32_t> transferFamily;

    bool isComplete() const;
    std::vector<uint32_t> getUniqueQueueFamilies();
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

#endif //CUBICAD_TYPES_H
