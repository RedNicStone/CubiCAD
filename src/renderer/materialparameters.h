//
// Created by nic on 23/01/2022.
//

#ifndef CUBICAD_MATERIALPARAMETERS_H
#define CUBICAD_MATERIALPARAMETERS_H

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>
#include <stdexcept>


enum MaterialPropertyInput {
    MATERIAL_PROPERTY_INPUT_CONSTANT = 0, MATERIAL_PROPERTY_INPUT_TEXTURE = 1 << 0,
};

enum MaterialPropertyFormat {
    MATERIAL_PROPERTY_FORMAT_UINT = 0,
    MATERIAL_PROPERTY_FORMAT_SINT = 1,
    MATERIAL_PROPERTY_FORMAT_UFLOAT = 2,
    MATERIAL_PROPERTY_FORMAT_SFLOAT = 3,
    MATERIAL_PROPERTY_FORMAT_UNORM = 4,
    MATERIAL_PROPERTY_FORMAT_SNORM = 5,
    MATERIAL_PROPERTY_FORMAT_USCALED = 6,
    MATERIAL_PROPERTY_FORMAT_SSCALED = 7,
    MATERIAL_PROPERTY_FORMAT_SRGB = 8,
    MATERIAL_PROPERTY_FORMAT_MAX_ENUM = 9,
};

enum MaterialPropertySize {
    MATERIAL_PROPERTY_SIZE_8 = 0,
    MATERIAL_PROPERTY_SIZE_16 = 1,
    MATERIAL_PROPERTY_SIZE_32 = 2,
    MATERIAL_PROPERTY_SIZE_64 = 3,
    MATERIAL_PROPERTY_SIZE_MAX_ENUM = 4,
};

enum MaterialPropertyCount {
    MATERIAL_PROPERTY_COUNT_1 = 0,
    MATERIAL_PROPERTY_COUNT_2 = 1,
    MATERIAL_PROPERTY_COUNT_3 = 2,
    MATERIAL_PROPERTY_COUNT_4 = 3,
    MATERIAL_PROPERTY_COUNT_MAX_ENUM = 4,
};

struct MaterialProperty {
    MaterialPropertyInput input;
    MaterialPropertyFormat format;
    MaterialPropertySize size;
    MaterialPropertyCount count;

    std::string attributeName;
};

struct MaterialPropertyLayout {
    std::vector<MaterialProperty> properties{};
};

struct MaterialPropertyBuiltGeneric {
    MaterialPropertyInput input{};
    VkFormat pixelFormat{};

    std::string attributeName{};

    [[nodiscard]] virtual void *allocate() const {};

    virtual void *cast(void *obj) const {};

    [[nodiscard]] virtual size_t getSize() const { return 0; }

    virtual ~MaterialPropertyBuiltGeneric() = default;
};

template<typename T>
struct MaterialPropertyBuilt : public MaterialPropertyBuiltGeneric {
    [[nodiscard]] void *allocate() const override { return new T; }

    void *cast(void *obj) const override { return static_cast<T *>(obj); }

    [[nodiscard]] size_t getSize() const override { return sizeof(T); }
};

struct MaterialPropertyLayoutBuilt {
    size_t totalSize;
    std::vector<MaterialPropertyBuiltGeneric *> properties{};
};

MaterialPropertyBuiltGeneric *buildProperty(const MaterialProperty& property);
MaterialPropertyLayoutBuilt buildLayout(const MaterialPropertyLayout &layout);

#endif //CUBICAD_MATERIALPARAMETERS_H
