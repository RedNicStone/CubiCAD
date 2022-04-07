//
// Created by nic on 08/03/2021.
//

#pragma once

#ifndef CUBICAD_COMPUTEPIPELINE_H
#define CUBICAD_COMPUTEPIPELINE_H

#include "device.h"
#include "pipelinebase.h"
#include "shader.h"

#include <vulkan/vulkan.h>

#include <vector>


class RenderPass;

class ComputePipeline : public PipelineBase {
  public:
    static std::shared_ptr<ComputePipeline> create(const std::shared_ptr<Device> &pDevice,
                                                   const std::shared_ptr<PipelineLayout> &pLayout,
                                                   const std::shared_ptr<ComputeShader> &shader,
                                                   const VkSpecializationInfo *shaderSpecialization = nullptr);

    VkPipelineBindPoint getBindPoint() final { return VK_PIPELINE_BIND_POINT_COMPUTE; }

    ~ComputePipeline() override;
};

#endif //CUBICAD_COMPUTEPIPELINE_H
