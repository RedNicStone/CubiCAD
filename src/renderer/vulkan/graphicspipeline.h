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
  public:
    static std::shared_ptr<GraphicsPipeline> create(std::shared_ptr<Device> pDevice,
                                                    std::shared_ptr<PipelineLayout> pLayout,
                                                    std::vector<std::shared_ptr<GraphicsShader>> &shaders,
                                                    const std::shared_ptr<RenderPass> &renderPass,
                                                    uint32_t colorBlendStates,
                                                    VkExtent2D extent,
                                                    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
                                                    VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
                                                    std::vector<VkVertexInputBindingDescription> bindingDescription = {},
                                                    std::vector<VkVertexInputAttributeDescription> attributeDescription = {},
                                                    bool enableDepthStencil = false);

    VkPipelineBindPoint getBindPoint() final { return VK_PIPELINE_BIND_POINT_GRAPHICS; }

    ~GraphicsPipeline() override;
};

#endif //CUBICAD_GRAPHICSPIPELINE_H
