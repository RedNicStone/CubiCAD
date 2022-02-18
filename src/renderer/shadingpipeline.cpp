//
// Created by nic on 16/02/2022.
//

#include "shadingpipeline.h"


std::shared_ptr<ShadingPipeline> ShadingPipeline::create(const std::shared_ptr<Device>& device) {
    auto shadingPipeline = std::make_shared<ShadingPipeline>();

    shadingPipeline->vertexShader = VertexShader::create(device, "main", "resources/compiled/quad.vert.spv");
    shadingPipeline->fragmentShader = FragmentShader::create(device, "main",
                                                             "resources/compiled/viewport_selector.frag.spv")
}
