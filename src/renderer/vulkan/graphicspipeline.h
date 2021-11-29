//
// Created by nic on 08/03/2021.
//

#pragma once

#ifndef CUBICAD_GRAPHICSPIPELINE_H
#define CUBICAD_GRAPHICSPIPELINE_H

#include "device.h"
#include "pipelinebase.h"
#include "shader.h"
#include "swapchain.h"
#include "renderpass.h"

#include <vulkan/vulkan.h>

#include <vector>


class RenderPass;

class GraphicsPipeline : public PipelineBase {
  private:
    VkQueue graphicsQueue;

  public:
    GraphicsPipeline(Device *device,
                     PipelineLayout *layout,
                     std::vector<GraphicsShader *> &shaders,
                     const RenderPass &renderPass,
                     VkExtent2D extent);

    VkPipelineBindPoint getBindPoint() final { return VK_PIPELINE_BIND_POINT_GRAPHICS; }
};

#endif //CUBICAD_GRAPHICSPIPELINE_H
