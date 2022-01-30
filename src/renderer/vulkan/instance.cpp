//
// Created by nic on 21/03/2021.
//

#include "instance.h"


void Instance::createPhysicalDevices() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(handle, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::cout << "\n\nFound " << deviceCount << " device[s]\n";

    std::vector<VkPhysicalDevice> possibleDevices(deviceCount);
    vkEnumeratePhysicalDevices(handle, &deviceCount, possibleDevices.data());

    VkPhysicalDeviceProperties physicalProperties;
    for (VkPhysicalDevice device: possibleDevices) {
        vkGetPhysicalDeviceProperties(device, &physicalProperties);
        std::cout << "  * " << physicalProperties.deviceName << std::endl;
    }

    physicalDevices.reserve(deviceCount);
    for (auto device: possibleDevices)
        physicalDevices.push_back(PhysicalDevice::create(shared_from_this(), device));
}

void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
}

VkResult Instance::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                    const VkAllocationCallbacks *pAllocator,
                                                    VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

bool Instance::checkExtensionSupport(bool validationLayerExtensions) {
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    std::vector<const char *> vecGlfwExtensions = getRequiredExtensions(validationLayerExtensions);

    for (const char *extensionName: vecGlfwExtensions) {
        bool extensionFound = false;

        for (const auto &extensionProperties: availableExtensions) {
            if (strcmp(extensionName, extensionProperties.extensionName) == 0) {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound) {
            return false;
        }
    }

    return true;
}

std::vector<VkLayerProperties> Instance::getSupportedLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    return availableLayers;
}

bool Instance::checkValidationLayerSupport(const std::vector<char *> &validationLayers) {
    std::vector<VkLayerProperties> availableLayers(getSupportedLayers());

    for (const auto layerName: validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties: availableLayers) {
            if (!strcmp(layerName, layerProperties.layerName)) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> Instance::getRequiredExtensions(bool validationLayerExtensions) {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (validationLayerExtensions) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

std::shared_ptr<Instance> Instance::create(const char *appName,
                                                   uint32_t appVersion,
                                                   bool shouldEnableValidationLayers) {
    auto instance = std::make_shared<Instance>();
    instance->enableValidationLayers = shouldEnableValidationLayers;

    instance->appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    instance->appInfo.apiVersion = VK_API_VERSION_1_2;
    instance->appInfo.pEngineName = "CubiCAD";
    instance->appInfo.engineVersion = 0;
    instance->appInfo.pApplicationName = appName;
    instance->appInfo.applicationVersion = appVersion;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &instance->appInfo;

    if (!Instance::checkExtensionSupport(instance->enableValidationLayers)) {
        throw std::runtime_error("not all required extensions available");
    }

    auto extensions = getRequiredExtensions(instance->enableValidationLayers);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (instance->enableValidationLayers) {
        instance->validationLayers.push_back((char *) "VK_LAYER_KHRONOS_validation");

        if (!Instance::checkValidationLayerSupport(instance->validationLayers)) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        createInfo.enabledLayerCount = static_cast<uint32_t>(instance->validationLayers.size());
        createInfo.ppEnabledLayerNames = instance->validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;

    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance->handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to crate vulkan instance!");
    }
    //todo add configurable extensions
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
    populateDebugMessengerCreateInfo(debugMessengerCreateInfo);

    if (CreateDebugUtilsMessengerEXT(instance->handle, &debugMessengerCreateInfo, nullptr, &instance->debugMessenger)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }

    instance->createPhysicalDevices();

    return instance;
}
