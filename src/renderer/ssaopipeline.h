//
// Created by nic on 18/03/2022.
//

#pragma once

#ifndef CUBICAD_SSAOPIPELINE_H
#define CUBICAD_SSAOPIPELINE_H

#include "vulkan/computepipeline.h"
#include "descriptorpoolmanager.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <random>


class ComputePipeline;

class SSAOPipeline {
  private:
    struct PassInfo {
        glm::uvec2 imageSize;
        std::vector<glm::vec4> samples;
    };

    VkExtent2D imageExtend;

    std::shared_ptr<DescriptorSet> sampleDescriptorSet;
    std::shared_ptr<UniformBuffer> sampleUniform;

    std::shared_ptr<Sampler> imageSampler;

    std::shared_ptr<ComputePipeline> obscurancePipeline;

  public:
    static std::shared_ptr<SSAOPipeline> create(const std::shared_ptr<Device> &pDevice,
                                                const std::shared_ptr<DescriptorPoolManager> &poolManager,
                                                const std::shared_ptr<Queue> &transferQueue,
                                                const std::shared_ptr<ImageView>
                                                &resultImageView,
                                                const std::shared_ptr<ImageView>
                                                &normalImageView,
                                                const std::shared_ptr<ImageView>
                                                &posImageView,
                                                VkExtent2D imageExtend,
                                                uint32_t sampleCount);

    void bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer);

};

#endif //CUBICAD_SSAOPIPELINE_H
