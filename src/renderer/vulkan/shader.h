//
// Created by nic on 21/05/2021.
//

#pragma once

#ifndef CUBICAD_SHADER_H
#define CUBICAD_SHADER_H

#include <vulkan/vulkan.h>

#include <cassert>
#include <stdexcept>
#include <utility>
#include <vector>
#include <string>

#include "device.h"


class Shader {
  protected:
    std::string name;

    VkShaderModule shaderModule{};
    std::vector<char> shaderCode;

    std::shared_ptr<Device> device;

    static std::shared_ptr<Shader> create(std::shared_ptr<Device> pDevice,
                                          std::string nName,
                                          const std::string &filename);

    VkShaderStageFlagBits shaderStage = VK_SHADER_STAGE_ALL;
  public:
    void loadModule();

    VkPipelineShaderStageCreateInfo pipelineStageInfo();

    virtual ~Shader();
};

class GraphicsShader : virtual public Shader {
  protected:
    static std::shared_ptr<GraphicsShader> create(std::shared_ptr<Device> pDevice,
                                                  std::string nName,
                                                  const std::string &filename) {
        auto shader = std::make_shared<GraphicsShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();

        return shader;
    }

};

class NVShader : virtual public Shader {
  protected:
    static std::shared_ptr<NVShader> create(std::shared_ptr<Device> pDevice,
                                            std::string nName,
                                            const std::string &filename) {
        auto shader = std::make_shared<NVShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();

        return shader;
    }

};

class KHRShader : virtual public Shader {
  protected:
    static std::shared_ptr<KHRShader> create(std::shared_ptr<Device> pDevice,
                                             std::string nName,
                                             const std::string &filename) {
        auto shader = std::make_shared<KHRShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();

        return shader;
    }

};

class RaytracingShader : virtual public GraphicsShader {
  protected:
    static std::shared_ptr<RaytracingShader> create(std::shared_ptr<Device> pDevice,
                                                    std::string nName,
                                                    const std::string &filename) {
        auto shader = std::make_shared<RaytracingShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();

        return shader;
    }

};

class VertexShader : virtual public GraphicsShader {
  public:
    static std::shared_ptr<VertexShader> create(std::shared_ptr<Device> pDevice,
                                                std::string nName,
                                                const std::string &filename) {
        auto shader = std::make_shared<VertexShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_VERTEX_BIT;

        return shader;
    }

};

class TesselationControlShader : virtual public GraphicsShader {
  public:
    static std::shared_ptr<TesselationControlShader> create(std::shared_ptr<Device> pDevice,
                                                            std::string nName,
                                                            const std::string &filename) {
        auto shader = std::make_shared<TesselationControlShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

        return shader;
    }

};

class TesselationEvaluationShader : virtual public GraphicsShader {
  public:
    static std::shared_ptr<TesselationEvaluationShader> create(std::shared_ptr<Device> pDevice,
                                                               std::string nName,
                                                               const std::string &filename) {
        auto shader = std::make_shared<TesselationEvaluationShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

        return shader;
    }

};

class GeometryShader : virtual public GraphicsShader {
  public:
    static std::shared_ptr<GeometryShader> create(std::shared_ptr<Device> pDevice,
                                                  std::string nName,
                                                  const std::string &filename) {
        auto shader = std::make_shared<GeometryShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;

        return shader;
    }

};

class FragmentShader : virtual public GraphicsShader {
  public:
    static std::shared_ptr<FragmentShader> create(std::shared_ptr<Device> pDevice,
                                                  std::string nName,
                                                  const std::string &filename) {
        auto shader = std::make_shared<FragmentShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;

        return shader;
    }

};

class ComputeShader : virtual public Shader {
  public:
    static std::shared_ptr<ComputeShader> create(std::shared_ptr<Device> pDevice,
                                                 std::string nName,
                                                 const std::string &filename) {
        auto shader = std::make_shared<ComputeShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;

        return shader;
    }

};

class NVTaskShader : virtual public NVShader, virtual public ComputeShader {
  public:
    static std::shared_ptr<NVTaskShader> create(std::shared_ptr<Device> pDevice,
                                                std::string nName,
                                                const std::string &filename) {
        auto shader = std::make_shared<NVTaskShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_TASK_BIT_NV;

        return shader;
    }

};

class NVMeshShader : virtual public NVShader, virtual public GraphicsShader {
  public:
    static std::shared_ptr<NVMeshShader> create(std::shared_ptr<Device> pDevice,
                                                std::string nName,
                                                const std::string &filename) {
        auto shader = std::make_shared<NVMeshShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_MESH_BIT_NV;

        return shader;
    }

};

class NVRaygenShader : virtual public NVShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<NVRaygenShader> create(std::shared_ptr<Device> pDevice,
                                                  std::string nName,
                                                  const std::string &filename) {
        auto shader = std::make_shared<NVRaygenShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_RAYGEN_BIT_NV;

        return shader;
    }

};

class NVAnyHitShader : virtual public NVShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<NVAnyHitShader> create(std::shared_ptr<Device> pDevice,
                                                  std::string nName,
                                                  const std::string &filename) {
        auto shader = std::make_shared<NVAnyHitShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_ANY_HIT_BIT_NV;

        return shader;
    }

};

class NVClosestHitShader : virtual public NVShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<NVClosestHitShader> create(std::shared_ptr<Device> pDevice,
                                                      std::string nName,
                                                      const std::string &filename) {
        auto shader = std::make_shared<NVClosestHitShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;

        return shader;
    }

};

class NVMissShader : virtual public NVShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<NVMissShader> create(std::shared_ptr<Device> pDevice,
                                                std::string nName,
                                                const std::string &filename) {
        auto shader = std::make_shared<NVMissShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_MISS_BIT_NV;

        return shader;
    }

};

class NVCallableShader : virtual public NVShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<NVCallableShader> create(std::shared_ptr<Device> pDevice,
                                                    std::string nName,
                                                    const std::string &filename) {
        auto shader = std::make_shared<NVCallableShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_CALLABLE_BIT_NV;

        return shader;
    }

};

class KHRRaygenShader : virtual public KHRShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<KHRRaygenShader> create(std::shared_ptr<Device> pDevice,
                                                   std::string nName,
                                                   const std::string &filename) {
        auto shader = std::make_shared<KHRRaygenShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

        return shader;
    }

};

class KHRAnyHitShader : virtual public KHRShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<KHRAnyHitShader> create(std::shared_ptr<Device> pDevice,
                                                   std::string nName,
                                                   const std::string &filename) {
        auto shader = std::make_shared<KHRAnyHitShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

        return shader;
    }

};

class KHRClosestHitShader : virtual public KHRShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<KHRClosestHitShader> create(std::shared_ptr<Device> pDevice,
                                                       std::string nName,
                                                       const std::string &filename) {
        auto shader = std::make_shared<KHRClosestHitShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

        return shader;
    }

};

class KHRMissShader : virtual public KHRShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<KHRMissShader> create(std::shared_ptr<Device> pDevice,
                                                 std::string nName,
                                                 const std::string &filename) {
        auto shader = std::make_shared<KHRMissShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_MISS_BIT_KHR;

        return shader;
    }

};

class KHRCallableShader : virtual public KHRShader, virtual public RaytracingShader {
  public:
    static std::shared_ptr<KHRCallableShader> create(std::shared_ptr<Device> pDevice,
                                                     std::string nName,
                                                     const std::string &filename) {
        auto shader = std::make_shared<KHRCallableShader>();
        shader->device = std::move(pDevice);
        shader->name = std::move(nName);
        shader->shaderCode = readFile(filename);
        shader->loadModule();
        shader->shaderStage = VK_SHADER_STAGE_CALLABLE_BIT_KHR;

        return shader;
    }

};

#endif //CUBICAD_SHADER_H
