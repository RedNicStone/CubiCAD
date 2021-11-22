//
// Created by nic on 14/06/2021.
//

#include "physicaldevice.h"


PhysicalDevice::PhysicalDevice(Instance *callingInstance, VkPhysicalDevice device) {
    handle = device;
    instance = callingInstance;
    queueFamilyHandler = std::make_shared<QueueFamilyHandler>(QueueFamilyHandler(this));
}

Instance *PhysicalDevice::getInstance() {
    return instance;
}

bool PhysicalDevice::hasRequiredExtensions(const std::vector<std::string> &requiredExtensions) {
    std::vector<VkExtensionProperties> availableExtensions(getSupportedExtensions());
    std::set<std::string> requiredExtensionsSet(requiredExtensions.begin(), requiredExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensionsSet.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

std::vector<VkExtensionProperties> PhysicalDevice::getSupportedExtensions() {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionCount, availableExtensions.data());

    return availableExtensions;
}

void PhysicalDevice::queryDeviceProperties() {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(handle, &deviceProperties);

    std::cout << "\t* Device: " << handle << ":";
    std::cout << "\n\t\t* Name: " << deviceProperties.deviceName;
    std::cout << "\n\t\t* Type: " << deviceProperties.deviceType;
    std::cout << "\n\t\t* ID: " << deviceProperties.deviceID;
    std::cout << "\n\t\t* Vendor ID: " << deviceProperties.vendorID;
    std::cout << "\n\t\t* Driver version: " << deviceProperties.driverVersion;
    std::cout << "\n\t\t* API version: " << deviceProperties.apiVersion;
    std::cout << "\n\t\t* Pipeline cache UUID: " << std::bitset<64>((uint64_t) deviceProperties.pipelineCacheUUID);
    std::cout << "\n\t\t* Sparse properties: ";
    std::cout
        << "\n\t\t\t* Standard 2D block shape: "
        << deviceProperties.sparseProperties.residencyStandard2DBlockShape;
    std::cout
        << "\n\t\t\t* Standard 2D multisample block shape: "
        << deviceProperties.sparseProperties.residencyStandard2DMultisampleBlockShape;
    std::cout
        << "\n\t\t\t* Standard 3D block shape: "
        << deviceProperties.sparseProperties.residencyStandard3DBlockShape;
    std::cout << "\n\t\t\t* Aligned MIP size: " << deviceProperties.sparseProperties.residencyAlignedMipSize;
    std::cout << "\n\t\t\t* Non resident strict:" << deviceProperties.sparseProperties.residencyNonResidentStrict;
    std::cout << "\n\t\t* Limits:";
    std::cout << "\n\t\t\t* Max image dimension 1D: " << deviceProperties.limits.maxImageDimension1D;
    std::cout << "\n\t\t\t* Max image dimension 2D: " << deviceProperties.limits.maxImageDimension2D;
    std::cout << "\n\t\t\t* Max image dimension 3D: " << deviceProperties.limits.maxImageDimension3D;
    std::cout << "\n\t\t\t* Max image dimension cube: " << deviceProperties.limits.maxImageDimensionCube;
    std::cout << "\n\t\t\t* Max image array layers: " << deviceProperties.limits.maxImageArrayLayers;
    std::cout << "\n\t\t\t* Max texel buffer elements: " << deviceProperties.limits.maxTexelBufferElements;
    std::cout << "\n\t\t\t* Max uniform buffer range: " << deviceProperties.limits.maxUniformBufferRange;
    std::cout << "\n\t\t\t* Max storage buffer range: " << deviceProperties.limits.maxStorageBufferRange;
    std::cout << "\n\t\t\t* Max push constants size: " << deviceProperties.limits.maxPushConstantsSize;
    std::cout << "\n\t\t\t* Max memory allocation count: " << deviceProperties.limits.maxMemoryAllocationCount;
    std::cout << "\n\t\t\t* Max sampler allocation count: " << deviceProperties.limits.maxSamplerAllocationCount;
    std::cout << "\n\t\t\t* Buffer image granularity: " << deviceProperties.limits.bufferImageGranularity;
    std::cout << "\n\t\t\t* Sparse address space size: " << deviceProperties.limits.sparseAddressSpaceSize;
    std::cout << "\n\t\t\t* Max bound descriptor sets: " << deviceProperties.limits.maxBoundDescriptorSets;
    std::cout
        << "\n\t\t\t* Max per stage descriptor samplers: "
        << deviceProperties.limits.maxPerStageDescriptorSamplers;
    std::cout
        << "\n\t\t\t* Max per stage descriptor uniform buffers: "
        << deviceProperties.limits.maxPerStageDescriptorUniformBuffers;
    std::cout
        << "\n\t\t\t* Max per stage descriptor storage buffers: "
        << deviceProperties.limits.maxPerStageDescriptorStorageBuffers;
    std::cout
        << "\n\t\t\t* Max per stage descriptor sampled images: "
        << deviceProperties.limits.maxPerStageDescriptorSampledImages;
    std::cout
        << "\n\t\t\t* Max per stage descriptor storage images: "
        << deviceProperties.limits.maxPerStageDescriptorStorageImages;
    std::cout
        << "\n\t\t\t* Max per stage descriptor input attachments: "
        << deviceProperties.limits.maxPerStageDescriptorInputAttachments;
    std::cout << "\n\t\t\t* Max per stage resources: " << deviceProperties.limits.maxPerStageResources;
    std::cout << "\n\t\t\t* Max descriptor set samplers: " << deviceProperties.limits.maxDescriptorSetSamplers;
    std::cout
        << "\n\t\t\t* Max descriptor set uniform buffers: "
        << deviceProperties.limits.maxDescriptorSetUniformBuffers;
    std::cout
        << "\n\t\t\t* Max descriptor set uniform buffers [dynamic]: "
        << deviceProperties.limits.maxDescriptorSetUniformBuffersDynamic;
    std::cout
        << "\n\t\t\t* Max descriptor set storage buffers: "
        << deviceProperties.limits.maxDescriptorSetStorageBuffers;
    std::cout
        << "\n\t\t\t* Max descriptor set storage buffers [dynamic]: "
        << deviceProperties.limits.maxDescriptorSetStorageBuffersDynamic;
    std::cout
        << "\n\t\t\t* Max descriptor set sampled images: "
        << deviceProperties.limits.maxDescriptorSetSampledImages;
    std::cout
        << "\n\t\t\t* Max descriptor set storage images: "
        << deviceProperties.limits.maxDescriptorSetStorageImages;
    std::cout
        << "\n\t\t\t* Max descriptor set input attachments: "
        << deviceProperties.limits.maxDescriptorSetInputAttachments;
    std::cout << "\n\t\t\t* Max vertex input attributes: " << deviceProperties.limits.maxVertexInputAttributes;
    std::cout
        << "\n\t\t\t* Max vertex input attribute offset: "
        << deviceProperties.limits.maxVertexInputAttributeOffset;
    std::cout << "\n\t\t\t* Max vertex input bindings: " << deviceProperties.limits.maxVertexInputBindings;
    std::cout << "\n\t\t\t* Max vertex input binding stride: " << deviceProperties.limits.maxVertexInputBindingStride;
    std::cout << "\n\t\t\t* Max vertex output components: " << deviceProperties.limits.maxVertexOutputComponents;
    std::cout
        << "\n\t\t\t* Max tessellation generation level: "
        << deviceProperties.limits.maxTessellationGenerationLevel;
    std::cout << "\n\t\t\t* Max tessellation patch size: " << deviceProperties.limits.maxTessellationPatchSize;
    std::cout
        << "\n\t\t\t* Max tessellation control per vertex input components: "
        << deviceProperties.limits.maxTessellationControlPerVertexInputComponents;
    std::cout
        << "\n\t\t\t* Max tessellation control per vertex output components: "
        << deviceProperties.limits.maxTessellationControlPerVertexOutputComponents;
    std::cout
        << "\n\t\t\t* Max tessellation control per patch output components: "
        << deviceProperties.limits.maxTessellationControlPerPatchOutputComponents;
    std::cout
        << "\n\t\t\t* Max tessellation control total output components: "
        << deviceProperties.limits.maxTessellationControlTotalOutputComponents;
    std::cout
        << "\n\t\t\t* Max tessellation evaluation input components: "
        << deviceProperties.limits.maxTessellationEvaluationInputComponents;
    std::cout
        << "\n\t\t\t* Max tessellation evaluation output components: "
        << deviceProperties.limits.maxTessellationEvaluationOutputComponents;
    std::cout << "\n\t\t\t* Max geometry shader invocations: " << deviceProperties.limits.maxGeometryShaderInvocations;
    std::cout << "\n\t\t\t* Max geometry input components: " << deviceProperties.limits.maxGeometryInputComponents;
    std::cout << "\n\t\t\t* Max geometry output components: " << deviceProperties.limits.maxGeometryOutputComponents;
    std::cout << "\n\t\t\t* Max geometry output vertices: " << deviceProperties.limits.maxGeometryOutputVertices;
    std::cout
        << "\n\t\t\t* Max geometry total output components: "
        << deviceProperties.limits.maxGeometryTotalOutputComponents;
    std::cout << "\n\t\t\t* Max fragment input components: " << deviceProperties.limits.maxFragmentInputComponents;
    std::cout << "\n\t\t\t* Max fragment output attachments: " << deviceProperties.limits.maxFragmentOutputAttachments;
    std::cout
        << "\n\t\t\t* Max fragment dual src attachments: "
        << deviceProperties.limits.maxFragmentDualSrcAttachments;
    std::cout
        << "\n\t\t\t* Max fragment combined output resources: "
        << deviceProperties.limits.maxFragmentCombinedOutputResources;
    std::cout << "\n\t\t\t* Max compute shared memory size: " << deviceProperties.limits.maxComputeSharedMemorySize;
    std::cout
        << "\n\t\t\t* Max compute work group count: "
        << deviceProperties.limits.maxComputeWorkGroupCount[0]
        << " x "
        << deviceProperties.limits.maxComputeWorkGroupCount[1]
        << " x "
        << deviceProperties.limits.maxComputeWorkGroupCount[2];
    std::cout
        << "\n\t\t\t* Max compute work group invocations: "
        << deviceProperties.limits.maxComputeWorkGroupInvocations;
    std::cout
        << "\n\t\t\t* Max compute work group size: "
        << deviceProperties.limits.maxComputeWorkGroupSize[0]
        << " x "
        << deviceProperties.limits.maxComputeWorkGroupSize[1]
        << " x "
        << deviceProperties.limits.maxComputeWorkGroupSize[2];
    std::cout << "\n\t\t\t* Sub pixel precision bits: " << deviceProperties.limits.subPixelPrecisionBits;
    std::cout << "\n\t\t\t* Sub texel precision bits: " << deviceProperties.limits.subTexelPrecisionBits;
    std::cout << "\n\t\t\t* Max mipmap precision bits: " << deviceProperties.limits.mipmapPrecisionBits;
    std::cout << "\n\t\t\t* Max draw indexed value count: " << deviceProperties.limits.maxDrawIndexedIndexValue;
    std::cout << "\n\t\t\t* Max draw indirect count: " << deviceProperties.limits.maxDrawIndirectCount;
    std::cout << "\n\t\t\t* Max sampler LOD bias: " << deviceProperties.limits.maxSamplerLodBias;
    std::cout << "\n\t\t\t* Max sampler anisotropy: " << deviceProperties.limits.maxSamplerAnisotropy;
    std::cout << "\n\t\t\t* Max viewports: " << deviceProperties.limits.maxViewports;
    std::cout
        << "\n\t\t\t* Max viewport dimensions: "
        << deviceProperties.limits.maxViewportDimensions[0]
        << " x "
        << deviceProperties.limits.maxViewportDimensions[1];
    std::cout
        << "\n\t\t\t* Viewport bounds range: "
        << deviceProperties.limits.viewportBoundsRange[0]
        << " - "
        << deviceProperties.limits.viewportBoundsRange[1];
    std::cout << "\n\t\t\t* Viewport sub pixel bits: " << deviceProperties.limits.viewportSubPixelBits;
    std::cout << "\n\t\t\t* Min memory map alignment: " << deviceProperties.limits.minMemoryMapAlignment;
    std::cout
        << "\n\t\t\t* Min texel buffer offset alignment: "
        << deviceProperties.limits.minTexelBufferOffsetAlignment;
    std::cout
        << "\n\t\t\t* Min uniform buffer offset alignment: "
        << deviceProperties.limits.minUniformBufferOffsetAlignment;
    std::cout
        << "\n\t\t\t* Min storage buffer offset alignment: "
        << deviceProperties.limits.minStorageBufferOffsetAlignment;
    std::cout << "\n\t\t\t* Min texel offset: " << deviceProperties.limits.minTexelOffset;
    std::cout << "\n\t\t\t* Max texel offset: " << deviceProperties.limits.maxTexelOffset;
    std::cout << "\n\t\t\t* Min texel gather offset: " << deviceProperties.limits.minTexelGatherOffset;
    std::cout << "\n\t\t\t* Max texel gather offset: " << deviceProperties.limits.maxTexelGatherOffset;
    std::cout << "\n\t\t\t* Min interpolation offset: " << deviceProperties.limits.minInterpolationOffset;
    std::cout << "\n\t\t\t* Max interpolation offset: " << deviceProperties.limits.minInterpolationOffset;
    std::cout
        << "\n\t\t\t* Sub pixel interpolation offset bits: "
        << deviceProperties.limits.subPixelInterpolationOffsetBits;
    std::cout << "\n\t\t\t* Max framebuffer width: " << deviceProperties.limits.maxFramebufferWidth;
    std::cout << "\n\t\t\t* Max framebuffer height: " << deviceProperties.limits.maxFramebufferHeight;
    std::cout << "\n\t\t\t* Max framebuffer layers: " << deviceProperties.limits.maxFramebufferLayers;
    std::cout << "\n\t\t\t* Framebuffer color sample counts: " << deviceProperties.limits.framebufferColorSampleCounts;
    std::cout << "\n\t\t\t* Framebuffer depth sample counts: " << deviceProperties.limits.framebufferDepthSampleCounts;
    std::cout
        << "\n\t\t\t* Framebuffer stencil sample counts: "
        << deviceProperties.limits.framebufferStencilSampleCounts;
    std::cout
        << "\n\t\t\t* Framebuffer no attachment sample counts: "
        << deviceProperties.limits.framebufferNoAttachmentsSampleCounts;
    std::cout << "\n\t\t\t* Max color attachments: " << deviceProperties.limits.maxColorAttachments;
    std::cout
        << "\n\t\t\t* Sampled image color sample counts: "
        << deviceProperties.limits.sampledImageColorSampleCounts;
    std::cout
        << "\n\t\t\t* Sampled image integer sample counts: "
        << deviceProperties.limits.sampledImageIntegerSampleCounts;
    std::cout
        << "\n\t\t\t* Sampled image depth sample counts: "
        << deviceProperties.limits.sampledImageDepthSampleCounts;
    std::cout
        << "\n\t\t\t* Sampled image stencil sample counts: "
        << deviceProperties.limits.sampledImageStencilSampleCounts;
    std::cout << "\n\t\t\t* Storage image sample counts: " << deviceProperties.limits.storageImageSampleCounts;
    std::cout << "\n\t\t\t* Max sample mask words: " << deviceProperties.limits.maxSampleMaskWords;
    std::cout << "\n\t\t\t* Timestamp compute and graphic: " << deviceProperties.limits.timestampComputeAndGraphics;
    std::cout << "\n\t\t\t* Timestamp period: " << deviceProperties.limits.timestampPeriod;
    std::cout << "\n\t\t\t* Max clip distances: " << deviceProperties.limits.maxClipDistances;
    std::cout << "\n\t\t\t* Max cull distances: " << deviceProperties.limits.maxCullDistances;
    std::cout
        << "\n\t\t\t* Max combined clip and cull distances: "
        << deviceProperties.limits.maxCombinedClipAndCullDistances;
    std::cout << "\n\t\t\t* Discrete queue priorities: " << deviceProperties.limits.discreteQueuePriorities;
    std::cout
        << "\n\t\t\t* Point size range: "
        << deviceProperties.limits.pointSizeRange[0]
        << " - "
        << deviceProperties.limits.pointSizeRange[1];
    std::cout << "\n\t\t\t* Point size granularity: " << deviceProperties.limits.pointSizeGranularity;
    std::cout
        << "\n\t\t\t* Line width range: "
        << deviceProperties.limits.lineWidthRange[0]
        << " - "
        << deviceProperties.limits.lineWidthRange[1];
    std::cout << "\n\t\t\t* Line width granularity: " << deviceProperties.limits.lineWidthGranularity;
    std::cout << "\n\t\t\t* Strict lines: " << deviceProperties.limits.strictLines;
    std::cout << "\n\t\t\t* Standard sample locations: " << deviceProperties.limits.standardSampleLocations;
    std::cout
        << "\n\t\t\t* Optimal buffer copy offset alignment: "
        << deviceProperties.limits.optimalBufferCopyOffsetAlignment;
    std::cout
        << "\n\t\t\t* Optimal buffer copy row pitch alignment: "
        << deviceProperties.limits.optimalBufferCopyRowPitchAlignment;
    std::cout << "\n\t\t\t* Non coherent atom size: " << deviceProperties.limits.nonCoherentAtomSize;
    std::cout << '\n';
}
