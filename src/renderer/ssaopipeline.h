//
// Created by nic on 18/03/2022.
//

#pragma once

#ifndef CUBICAD_SSAOPIPELINE_H
#define CUBICAD_SSAOPIPELINE_H

#include "vulkan/computepipeline.h"
#include "descriptorpoolmanager.h"
#include "camera.h"
#include "../utils/utils.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <random>


class ComputePipeline;

class SSAOPipeline {
  private:
    struct PassInfo {
        alignas(8) glm::uvec2 imageSize{};
        alignas(4) float tanFOV{};
    };

    VkExtent2D imageExtend;

    std::shared_ptr<DescriptorSet> sceneDescriptorSet;

    std::shared_ptr<DescriptorSet> sampleDescriptorSet;
    std::shared_ptr<UniformBuffer> sampleUniform;

    std::shared_ptr<Image> noiseImage;
    std::shared_ptr<ImageView> noiseImageView;
    std::shared_ptr<Sampler> imageSampler;

    std::shared_ptr<ComputePipeline> obscurancePipeline;
    std::shared_ptr<ComputePipeline> blurPipeline;

  public:
    static std::shared_ptr<SSAOPipeline> create(const std::shared_ptr<Device> &pDevice,
                                                const std::shared_ptr<DescriptorPoolManager> &poolManager,
                                                const std::shared_ptr<CommandPool> &transferPool,
                                                const std::shared_ptr<CommandPool> &computePool,
                                                const std::shared_ptr<ImageView>
                                                &resultImageView,
                                                const std::shared_ptr<ImageView>
                                                &normalImageView,
                                                const std::shared_ptr<ImageView>
                                                &posImageView,
                                                const std::shared_ptr<DescriptorSet>& sceneDescriptor,
                                                float fov,
                                                VkExtent2D imageExtend,
                                                uint32_t sampleCount);

    void bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer);

};

#endif //CUBICAD_SSAOPIPELINE_H
