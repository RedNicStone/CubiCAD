//
// Created by nic on 23/01/2022.
//

#ifndef CUBICAD_MATERIALPARAMETERS_H
#define CUBICAD_MATERIALPARAMETERS_H

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>
#include <stdexcept>
#include <memory>


/// Enumeration for shader input types
enum MaterialPropertyInput : uint32_t {
    MATERIAL_PROPERTY_INPUT_CONSTANT = 1,
    MATERIAL_PROPERTY_INPUT_TEXTURE = 2,
};

typedef uint32_t MaterialPropertyInputFlag;

/// Enumeration for shader input formats
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

/// Enumeration for shader input sizes
enum MaterialPropertySize {
    MATERIAL_PROPERTY_SIZE_8 = 0,
    MATERIAL_PROPERTY_SIZE_16 = 1,
    MATERIAL_PROPERTY_SIZE_32 = 2,
    MATERIAL_PROPERTY_SIZE_64 = 3,
    MATERIAL_PROPERTY_SIZE_MAX_ENUM = 4,
};

/// Enumeration for shader input length
enum MaterialPropertyCount {
    MATERIAL_PROPERTY_COUNT_1 = 0,
    MATERIAL_PROPERTY_COUNT_2 = 1,
    MATERIAL_PROPERTY_COUNT_3 = 2,
    MATERIAL_PROPERTY_COUNT_4 = 3,
    MATERIAL_PROPERTY_COUNT_MAX_ENUM = 4,
};

/// Structure storing a singular shader input
struct MaterialProperty {
    MaterialPropertyInputFlag input;
    MaterialPropertyFormat format;
    MaterialPropertySize size;
    MaterialPropertyCount count;

    std::string attributeName;
};

/// Structure storing a layout of multiple shader inputs
struct MaterialPropertyLayout {
    std::vector<MaterialProperty> properties{};
};

/// Built version of MaterialProperty that has no type associated
struct MaterialPropertyBuiltGeneric {
    MaterialPropertyInputFlag input{};
    VkFormat pixelFormat{};
    size_t size{};

    std::string attributeName{};

    [[nodiscard]] virtual void *allocate() const {};

    virtual void *cast(void *obj) const {};

    [[nodiscard]] virtual size_t getSize() const { return size; }

    virtual ~MaterialPropertyBuiltGeneric() = default;
};

/// Built version of MaterialProperty that has a type associated
template<typename T>
struct MaterialPropertyBuilt : public MaterialPropertyBuiltGeneric {
    MaterialPropertyBuilt() {
        size = sizeof(T);
    }

    [[nodiscard]] void *allocate() const override { return new T; }

    void *cast(void *obj) const override { return static_cast<T *>(obj); }
};

/// Built version of MaterialPropertyLayout which contains a list of built shader properties
struct MaterialPropertyLayoutBuilt {
    size_t totalSize;
    std::vector<MaterialPropertyBuiltGeneric *> properties{};

    ~MaterialPropertyLayoutBuilt() {
        for (const auto property : properties)
            delete property;
    }
};

/// Build a property
/// \param property Property to build
/// \return Generified cast of build material with type associated
MaterialPropertyBuiltGeneric *buildProperty(const MaterialProperty& property);
/// Build whole property layout
/// \param layout Layout to build
/// \return Build layout
std::shared_ptr<MaterialPropertyLayoutBuilt> buildLayout(const MaterialPropertyLayout &layout);

/// Copy a built layout
/// \param layout Original layout
/// \return Independent copy
std::shared_ptr<MaterialPropertyLayoutBuilt> copyLayout(const std::shared_ptr<MaterialPropertyLayoutBuilt>& layout);

#endif //CUBICAD_MATERIALPARAMETERS_H
