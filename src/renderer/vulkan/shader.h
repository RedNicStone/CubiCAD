//
// Created by nic on 21/05/2021.
//

#pragma once

#ifndef CUBICAD_SHADER_H
#define CUBICAD_SHADER_H

#include <vulkan/vulkan.h>

#include <cassert>
#include <stdexcept>
#include <vector>
#include <string>

#include "device.h"


class Shader {
  private:
    std::string name;

    VkShaderModule shaderModule{};
    std::vector<char> shaderCode;

    Device *device;

  protected:
    Shader(Device *device, std::string name, const std::string &filename);

    VkShaderStageFlagBits shaderStage = VK_SHADER_STAGE_ALL;
  public:
    void loadModule();

    VkPipelineShaderStageCreateInfo pipelineStageInfo();

    virtual ~Shader();
};

class GraphicsShader : public Shader {
  protected:
    GraphicsShader(Device *device, std::string name, const std::string &filename) : Shader(device,
                                                                                           std::move(name),
                                                                                           filename) {};

};

class NVShader : public Shader {
  protected:
    NVShader(Device *device, std::string name, const std::string &filename) : Shader(device,
                                                                                     std::move(name),
                                                                                     filename) {};

};

class KHRShader : public Shader {
  protected:
    KHRShader(Device *device, std::string name, const std::string &filename) : Shader(device,
                                                                                      std::move(name),
                                                                                      filename) {};

};

class RaytracingShader : public GraphicsShader {
  protected:
    RaytracingShader(Device *device, std::string name, const std::string &filename) : GraphicsShader(device,
                                                                                                     std::move(name),
                                                                                                     filename) {};

};

class VertexShader : public GraphicsShader {
  public:
    VertexShader(Device *device, std::string name, const std::string &filename) : GraphicsShader(device,
                                                                                                 std::move(name),
                                                                                                 filename) {
        shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
    }

};

class TesselationControlShader : public GraphicsShader {
  public:
    TesselationControlShader(Device *device, std::string name, const std::string &filename) : GraphicsShader(device,
                                                                                                             std::move(
                                                                                                                 name),
                                                                                                             filename) {
        shaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    }

};

class TesselationEvaluationShader : public GraphicsShader {
  public:
    TesselationEvaluationShader(Device *device, std::string name, const std::string &filename) : GraphicsShader(device,
                                                                                                                std::move(
                                                                                                                    name),
                                                                                                                filename) {
        shaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }

};

class GeometryShader : public GraphicsShader {
  public:
    GeometryShader(Device *device, std::string name, const std::string &filename) : GraphicsShader(device,
                                                                                                   std::move(name),
                                                                                                   filename) {
        shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
    }

};

class FragmentShader : public GraphicsShader {
  public:
    FragmentShader(Device *device, std::string name, const std::string &filename) : GraphicsShader(device,
                                                                                                   std::move(name),
                                                                                                   filename) {
        shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

};

class ComputeShader : public Shader {
  public:
    ComputeShader(Device *device, std::string name, const std::string &filename) : Shader(device,
                                                                                          std::move(name),
                                                                                          filename) {
        shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
    }

};

class NVTaskShader : public NVShader, public ComputeShader {
  public:
    NVTaskShader(Device *device, std::string name, const std::string &filename) : NVShader(device,
                                                                                           std::move(name),
                                                                                           filename),
                                                                                  ComputeShader(device,
                                                                                                std::move(name),
                                                                                                filename) {
        ComputeShader::shaderStage = VK_SHADER_STAGE_TASK_BIT_NV;
    }

};

class NVMeshShader : public NVShader, public GraphicsShader {
  public:
    NVMeshShader(Device *device, std::string name, const std::string &filename) : NVShader(device,
                                                                                           std::move(name),
                                                                                           filename),
                                                                                  GraphicsShader(device,
                                                                                                 std::move(name),
                                                                                                 filename) {
        GraphicsShader::shaderStage = VK_SHADER_STAGE_MESH_BIT_NV;
    }

};

class NVRaygenShader : public NVShader, public RaytracingShader {
  public:
    NVRaygenShader(Device *device, std::string name, const std::string &filename) : NVShader(device,
                                                                                             std::move(name),
                                                                                             filename),
                                                                                    RaytracingShader(device,
                                                                                                     std::move(name),
                                                                                                     filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_RAYGEN_BIT_NV;
    }

};

class NVAnyHitShader : public NVShader, public RaytracingShader {
  public:
    NVAnyHitShader(Device *device, std::string name, const std::string &filename) : NVShader(device,
                                                                                             std::move(name),
                                                                                             filename),
                                                                                    RaytracingShader(device,
                                                                                                     std::move(name),
                                                                                                     filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_ANY_HIT_BIT_NV;
    }

};

class NVClosestHitShader : public NVShader, public RaytracingShader {
  public:
    NVClosestHitShader(Device *device, std::string name, const std::string &filename) : NVShader(device,
                                                                                                 std::move(name),
                                                                                                 filename),
                                                                                        RaytracingShader(device,
                                                                                                         std::move(name),
                                                                                                         filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;
    }

};

class NVMissShader : public NVShader, public RaytracingShader {
  public:
    NVMissShader(Device *device, std::string name, const std::string &filename) : NVShader(device,
                                                                                           std::move(name),
                                                                                           filename),
                                                                                  RaytracingShader(device,
                                                                                                   std::move(name),
                                                                                                   filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_MISS_BIT_NV;
    }

};

class NVCallableShader : public NVShader, public RaytracingShader {
  public:
    NVCallableShader(Device *device, std::string name, const std::string &filename) : NVShader(device,
                                                                                               std::move(name),
                                                                                               filename),
                                                                                      RaytracingShader(device,
                                                                                                       std::move(name),
                                                                                                       filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_CALLABLE_BIT_NV;
    }

};

class KHRRaygenShader : public KHRShader, public RaytracingShader {
  public:
    KHRRaygenShader(Device *device, std::string name, const std::string &filename) : KHRShader(device,
                                                                                               std::move(name),
                                                                                               filename),
                                                                                     RaytracingShader(device,
                                                                                                      std::move(name),
                                                                                                      filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    }

};

class KHRAnyHitShader : public KHRShader, public RaytracingShader {
  public:
    KHRAnyHitShader(Device *device, std::string name, const std::string &filename) : KHRShader(device,
                                                                                               std::move(name),
                                                                                               filename),
                                                                                     RaytracingShader(device,
                                                                                                      std::move(name),
                                                                                                      filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    }

};

class KHRClosestHitShader : public KHRShader, public RaytracingShader {
  public:
    KHRClosestHitShader(Device *device, std::string name, const std::string &filename) : KHRShader(device,
                                                                                                   std::move(name),
                                                                                                   filename),
                                                                                         RaytracingShader(device,
                                                                                                          std::move(name),
                                                                                                          filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    }

};

class KHRMissShader : public KHRShader, public RaytracingShader {
  public:
    KHRMissShader(Device *device, std::string name, const std::string &filename) : KHRShader(device,
                                                                                             std::move(name),
                                                                                             filename),
                                                                                   RaytracingShader(device,
                                                                                                    std::move(name),
                                                                                                    filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_MISS_BIT_KHR;
    }

};

class KHRCallableShader : public KHRShader, public RaytracingShader {
  public:
    KHRCallableShader(Device *device, std::string name, const std::string &filename) : KHRShader(device,
                                                                                                 std::move(name),
                                                                                                 filename),
                                                                                       RaytracingShader(device,
                                                                                                        std::move(name),
                                                                                                        filename) {
        RaytracingShader::shaderStage = VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    }

};

#endif //CUBICAD_SHADER_H
