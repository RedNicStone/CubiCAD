//
// Created by nic on 31/12/2021.
//

#ifndef CUBICAD_MASTERMATERIAL_H
#define CUBICAD_MASTERMATERIAL_H

#include "vulkan/shader.h"


enum SHADER_PARAMETER : uint8_t {
    SHADER_PARAMETER_TYPE_CONSTANT = 0,
    SHADER_PARAMETER_TYPE_TEXTURE  = 1,
    SHADER_PARAMETER_TYPE_BUFFER   = 1,
};

struct ShaderParameter {
    static const SHADER_PARAMETER parameterType;
};

template<typename Type, uint32_t typeCount>
struct ShaderParameterConstant : public ShaderParameter {
    static const SHADER_PARAMETER parameterType = SHADER_PARAMETER_TYPE_CONSTANT;
    std::array<Type, typeCount> value;
};

struct ShaderParameterTexture : public ShaderParameter {
    static const SHADER_PARAMETER parameterType = SHADER_PARAMETER_TYPE_TEXTURE;
    VkFormat imageFormat;
    std::string path;
};

struct ShaderParameterBuffer : public ShaderParameter {
    static const SHADER_PARAMETER parameterType = SHADER_PARAMETER_TYPE_TEXTURE;
    VkFormat imageFormat;
    std::string path;
};

class MasterMaterial {
  private:
    std::vector<std::shared_ptr<GraphicsShader>> shaders;
    std::vector<ShaderParameter> parameters;

  public:

};

#endif //CUBICAD_MASTERMATERIAL_H
