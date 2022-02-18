//
// Created by nic on 16/02/2022.
//

#pragma once

#ifndef CUBICAD_SHADINGPIPELINE_H
#define CUBICAD_SHADINGPIPELINE_H

#include "vulkan/graphicspipeline.h"

class ShadingPipeline {
  private:
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;
    std::shared_ptr<GraphicsPipeline> pipeline;

  public:
    static std::shared_ptr<ShadingPipeline> create();

    void bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer);
};

#endif //CUBICAD_SHADINGPIPELINE_H
