//
// Created by nic on 23/01/2022.
//

#define CUBICAD_MATERIALPARAMETERS_H
#ifndef CUBICAD_MATERIALPARAMETERS_H

#include <vulkan/vulkan.h>
#include <vector>


enum MaterialPropertyInput {
    MATERIAL_PROPERTY_INPUT_CONSTANT = 0,
    MATERIAL_PROPERTY_INPUT_TEXTURE  = 1 << 0,
};

enum MaterialPropertyFormat {
    MATERIAL_PROPERTY_FORMAT_UINT     = 0,
    MATERIAL_PROPERTY_FORMAT_SINT     = 1,
    MATERIAL_PROPERTY_FORMAT_UFLOAT   = 2,
    MATERIAL_PROPERTY_FORMAT_SFLOAT   = 3,
    MATERIAL_PROPERTY_FORMAT_UNORM    = 4,
    MATERIAL_PROPERTY_FORMAT_SNORM    = 5,
    MATERIAL_PROPERTY_FORMAT_USCALED  = 6,
    MATERIAL_PROPERTY_FORMAT_SSCALED  = 7,
    MATERIAL_PROPERTY_FORMAT_SRGB     = 8,
    MATERIAL_PROPERTY_FORMAT_MAX_ENUM = 9,
};

enum MaterialPropertySize {
    MATERIAL_PROPERTY_SIZE_8         = 0,
    MATERIAL_PROPERTY_SIZE_16        = 1,
    MATERIAL_PROPERTY_SIZE_32        = 2,
    MATERIAL_PROPERTY_SIZE_64        = 3,
    MATERIAL_PROPERTY_SIZE_MAX_ENUM  = 4,
};

enum MaterialPropertyCount {
    MATERIAL_PROPERTY_COUNT_1        = 0,
    MATERIAL_PROPERTY_COUNT_2        = 1,
    MATERIAL_PROPERTY_COUNT_3        = 2,
    MATERIAL_PROPERTY_COUNT_4        = 3,
    MATERIAL_PROPERTY_COUNT_MAX_ENUM = 4,
};

struct MaterialProperty {
    MaterialPropertyInput input;
    MaterialPropertyFormat format;
    MaterialPropertySize size;
    MaterialPropertyCount count;
};

struct MaterialPropertyLayout {
    std::vector<MaterialProperty> properties{};
};

struct MaterialPropertyBuiltGeneric {
    MaterialPropertyInput input{};
    VkFormat pixelFormat{};

    [[nodiscard]] virtual void* allocate() const {};
    virtual void* cast(void* obj) const {};
    [[nodiscard]] virtual size_t getSize() const { return 0; }

    virtual ~MaterialPropertyBuiltGeneric() = default;
};

template<typename T>
struct MaterialPropertyBuilt : public MaterialPropertyBuiltGeneric {
    [[nodiscard]] void* allocate() const override { return new T; }
    void* cast(void* obj) const override { return static_cast<T*>(obj); }
    [[nodiscard]] size_t getSize() const override { return sizeof(T); }
};

struct MaterialPropertyLayoutBuilt {
    size_t totalSize;
    std::vector<MaterialPropertyBuiltGeneric*> properties{};
};

MaterialPropertyBuiltGeneric* buildProperty(MaterialProperty property) {
    VkFormat pixelFormat = VK_FORMAT_UNDEFINED;
    if (property.input == MATERIAL_PROPERTY_INPUT_TEXTURE) {
        switch (property.size) {
            case MATERIAL_PROPERTY_SIZE_8:
                pixelFormat = static_cast<VkFormat>(9);
                switch (property.format) {
                    case MATERIAL_PROPERTY_FORMAT_UINT:
                        pixelFormat = static_cast<VkFormat>(4 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SINT:
                        pixelFormat = static_cast<VkFormat>(5 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_UNORM:
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SNORM:
                        pixelFormat = static_cast<VkFormat>(1 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_USCALED:
                        pixelFormat = static_cast<VkFormat>(2 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SSCALED:
                        pixelFormat = static_cast<VkFormat>(3 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SRGB:
                        pixelFormat = static_cast<VkFormat>(6 + pixelFormat);
                        break;
                    default:
                        return nullptr;
                }
                pixelFormat = static_cast<VkFormat>(property.count * 7 + pixelFormat);
                if (property.count == MATERIAL_PROPERTY_COUNT_4)
                    pixelFormat = static_cast<VkFormat>(7 + pixelFormat);
                break;
            case MATERIAL_PROPERTY_SIZE_16:
                pixelFormat = static_cast<VkFormat>(70);
                switch (property.format) {
                    case MATERIAL_PROPERTY_FORMAT_UINT:
                        pixelFormat = static_cast<VkFormat>(4 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SINT:
                        pixelFormat = static_cast<VkFormat>(5 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SFLOAT:
                        pixelFormat = static_cast<VkFormat>(6 + pixelFormat);
                    case MATERIAL_PROPERTY_FORMAT_UNORM:
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SNORM:
                        pixelFormat = static_cast<VkFormat>(1 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_USCALED:
                        pixelFormat = static_cast<VkFormat>(2 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SSCALED:
                        pixelFormat = static_cast<VkFormat>(3 + pixelFormat);
                        break;
                    default:
                        return nullptr;
                }
                pixelFormat = static_cast<VkFormat>(property.count * 7 + pixelFormat);
                break;
            case MATERIAL_PROPERTY_SIZE_32:
                pixelFormat = static_cast<VkFormat>(98);
                switch (property.format) {
                    case MATERIAL_PROPERTY_FORMAT_UINT:
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SINT:
                        pixelFormat = static_cast<VkFormat>(1 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SFLOAT:
                        pixelFormat = static_cast<VkFormat>(2 + pixelFormat);
                        break;
                    default:
                        return nullptr;
                }
                pixelFormat = static_cast<VkFormat>(property.count * 3 + pixelFormat);
                break;
            case MATERIAL_PROPERTY_SIZE_64:
                pixelFormat = static_cast<VkFormat>(110);
                switch (property.format) {
                    case MATERIAL_PROPERTY_FORMAT_UINT:
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SINT:
                        pixelFormat = static_cast<VkFormat>(1 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SFLOAT:
                        pixelFormat = static_cast<VkFormat>(2 + pixelFormat);
                        break;
                    default:
                        return nullptr;
                }
                pixelFormat = static_cast<VkFormat>(property.count * 3 + pixelFormat);
                break;
            case MATERIAL_PROPERTY_SIZE_MAX_ENUM:
                return nullptr;
        }
    }
    MaterialPropertyBuiltGeneric* built;
    if (property.input == MATERIAL_PROPERTY_INPUT_CONSTANT) {
        switch (property.format) {
            case MATERIAL_PROPERTY_FORMAT_USCALED:
            case MATERIAL_PROPERTY_FORMAT_UINT:
                switch (property.size) {
                    case MATERIAL_PROPERTY_SIZE_8:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::u8vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::u8vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::u8vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::u8vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_16:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::u16vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::u16vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::u16vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::u16vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_32:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::u32vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::u32vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::u32vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::u32vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_64:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::u64vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::u64vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::u64vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::u64vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    default:
                        return nullptr;
                }
                break;
            case MATERIAL_PROPERTY_FORMAT_SINT:
            case MATERIAL_PROPERTY_FORMAT_SSCALED:
                switch (property.size) {
                    case MATERIAL_PROPERTY_SIZE_8:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::i8vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::i8vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::i8vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::i8vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_16:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::i16vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::i16vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::i16vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::i16vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_32:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::i32vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::i32vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::i32vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::i32vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_64:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::i64vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::i64vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::i64vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::i64vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    default:
                        return nullptr;
                }
                break;
            case MATERIAL_PROPERTY_FORMAT_SRGB:
            case MATERIAL_PROPERTY_FORMAT_UNORM:
            case MATERIAL_PROPERTY_FORMAT_SNORM:
            case MATERIAL_PROPERTY_FORMAT_UFLOAT:
            case MATERIAL_PROPERTY_FORMAT_SFLOAT:
                switch (property.size) {
                    case MATERIAL_PROPERTY_SIZE_8:
                    case MATERIAL_PROPERTY_SIZE_16:
                    case MATERIAL_PROPERTY_SIZE_32:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::f32vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::f32vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::f32vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::f32vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_64:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:
                                built = new MaterialPropertyBuilt<glm::f64vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:
                                built = new MaterialPropertyBuilt<glm::f64vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:
                                built = new MaterialPropertyBuilt<glm::f64vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:
                                built = new MaterialPropertyBuilt<glm::f64vec4>();
                                break;
                            default:
                                return nullptr;
                        }
                        break;
                    default:
                        return nullptr;
                }
                break;
            case MATERIAL_PROPERTY_FORMAT_MAX_ENUM:
                return nullptr;
        }
    } else
        built = new MaterialPropertyBuiltGeneric();

    built->input = property.input;
    built->pixelFormat = pixelFormat;

    return built;
}

MaterialPropertyLayoutBuilt buildLayout(const MaterialPropertyLayout& layout) {
    MaterialPropertyLayoutBuilt layoutBuilt{};
    size_t totalSize = 0;
    for (const auto& property : layout.properties) {
        auto builtProperty = buildProperty(property);
        if (builtProperty == nullptr)
            throw std::runtime_error("Unable to build material property");
        layoutBuilt.properties.push_back(builtProperty);
        totalSize += builtProperty->getSize();
    }
    layoutBuilt.totalSize = totalSize;
    return layoutBuilt;
}

#endif //CUBICAD_MATERIALPARAMETERS_H
