//
// Created by nic on 24/01/2022.
//

#include "materialparameters.h"


MaterialPropertyBuiltGeneric *buildProperty(const MaterialProperty& property) {
    VkFormat pixelFormat = VK_FORMAT_UNDEFINED;
    if (property.input & MATERIAL_PROPERTY_INPUT_TEXTURE) {
        switch (property.size) {
            case MATERIAL_PROPERTY_SIZE_8:pixelFormat = static_cast<VkFormat>(9);
                switch (property.format) {
                    case MATERIAL_PROPERTY_FORMAT_UINT:pixelFormat = static_cast<VkFormat>(4 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SINT:pixelFormat = static_cast<VkFormat>(5 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_UNORM:break;
                    case MATERIAL_PROPERTY_FORMAT_SNORM:pixelFormat = static_cast<VkFormat>(1 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_USCALED:pixelFormat = static_cast<VkFormat>(2 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SSCALED:pixelFormat = static_cast<VkFormat>(3 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SRGB:pixelFormat = static_cast<VkFormat>(6 + pixelFormat);
                        break;
                    default:return nullptr;
                }
                pixelFormat = static_cast<VkFormat>(property.count * 7 + pixelFormat);
                if (property.count == MATERIAL_PROPERTY_COUNT_4)
                    pixelFormat = static_cast<VkFormat>(7 + pixelFormat);
                break;
            case MATERIAL_PROPERTY_SIZE_16:pixelFormat = static_cast<VkFormat>(70);
                switch (property.format) {
                    case MATERIAL_PROPERTY_FORMAT_UINT:pixelFormat = static_cast<VkFormat>(4 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SINT:pixelFormat = static_cast<VkFormat>(5 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SFLOAT:pixelFormat = static_cast<VkFormat>(6 + pixelFormat);
                    case MATERIAL_PROPERTY_FORMAT_UNORM:break;
                    case MATERIAL_PROPERTY_FORMAT_SNORM:pixelFormat = static_cast<VkFormat>(1 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_USCALED:pixelFormat = static_cast<VkFormat>(2 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SSCALED:pixelFormat = static_cast<VkFormat>(3 + pixelFormat);
                        break;
                    default:return nullptr;
                }
                pixelFormat = static_cast<VkFormat>(property.count * 7 + pixelFormat);
                break;
            case MATERIAL_PROPERTY_SIZE_32:pixelFormat = static_cast<VkFormat>(98);
                switch (property.format) {
                    case MATERIAL_PROPERTY_FORMAT_UINT:break;
                    case MATERIAL_PROPERTY_FORMAT_SINT:pixelFormat = static_cast<VkFormat>(1 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SFLOAT:pixelFormat = static_cast<VkFormat>(2 + pixelFormat);
                        break;
                    default:return nullptr;
                }
                pixelFormat = static_cast<VkFormat>(property.count * 3 + pixelFormat);
                break;
            case MATERIAL_PROPERTY_SIZE_64:pixelFormat = static_cast<VkFormat>(110);
                switch (property.format) {
                    case MATERIAL_PROPERTY_FORMAT_UINT:break;
                    case MATERIAL_PROPERTY_FORMAT_SINT:pixelFormat = static_cast<VkFormat>(1 + pixelFormat);
                        break;
                    case MATERIAL_PROPERTY_FORMAT_SFLOAT:pixelFormat = static_cast<VkFormat>(2 + pixelFormat);
                        break;
                    default:return nullptr;
                }
                pixelFormat = static_cast<VkFormat>(property.count * 3 + pixelFormat);
                break;
            case MATERIAL_PROPERTY_SIZE_MAX_ENUM:return nullptr;
        }
    }
    MaterialPropertyBuiltGeneric *built;
    if (property.input & MATERIAL_PROPERTY_INPUT_CONSTANT) {
        switch (property.format) {
            case MATERIAL_PROPERTY_FORMAT_USCALED:
            case MATERIAL_PROPERTY_FORMAT_UINT:
                switch (property.size) {
                    case MATERIAL_PROPERTY_SIZE_8:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::u8vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::u8vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::u8vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::u8vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_16:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::u16vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::u16vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::u16vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::u16vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_32:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::u32vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::u32vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::u32vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::u32vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_64:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::u64vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::u64vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::u64vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::u64vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    default:return nullptr;
                }
                break;
            case MATERIAL_PROPERTY_FORMAT_SINT:
            case MATERIAL_PROPERTY_FORMAT_SSCALED:
                switch (property.size) {
                    case MATERIAL_PROPERTY_SIZE_8:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::i8vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::i8vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::i8vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::i8vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_16:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::i16vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::i16vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::i16vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::i16vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_32:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::i32vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::i32vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::i32vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::i32vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_64:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::i64vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::i64vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::i64vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::i64vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    default:return nullptr;
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
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::f32vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::f32vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::f32vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::f32vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    case MATERIAL_PROPERTY_SIZE_64:
                        switch (property.count) {
                            case MATERIAL_PROPERTY_COUNT_1:built = new MaterialPropertyBuilt<glm::f64vec1>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_2:built = new MaterialPropertyBuilt<glm::f64vec2>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_3:built = new MaterialPropertyBuilt<glm::f64vec3>();
                                break;
                            case MATERIAL_PROPERTY_COUNT_4:built = new MaterialPropertyBuilt<glm::f64vec4>();
                                break;
                            default:return nullptr;
                        }
                        break;
                    default:return nullptr;
                }
                break;
            case MATERIAL_PROPERTY_FORMAT_MAX_ENUM:return nullptr;
        }
    } else
        built = new MaterialPropertyBuiltGeneric();

    built->input = property.input;
    built->pixelFormat = pixelFormat;
    built->attributeName = property.attributeName;

    return built;
}

std::shared_ptr<MaterialPropertyLayoutBuilt> buildLayout(const MaterialPropertyLayout &layout) {
    std::shared_ptr<MaterialPropertyLayoutBuilt> layoutBuilt = std::make_shared<MaterialPropertyLayoutBuilt>();
    size_t totalSize = 0;
    for (const auto &property: layout.properties) {
        auto builtProperty = buildProperty(property);
        if (builtProperty == nullptr)
            throw std::runtime_error("Unable to build material property");
        layoutBuilt->properties.push_back(builtProperty);
        totalSize += builtProperty->getSize();
    }
    layoutBuilt->totalSize = totalSize;
    return layoutBuilt;
}

std::shared_ptr<MaterialPropertyLayoutBuilt> copyLayout(const std::shared_ptr<MaterialPropertyLayoutBuilt>& layout) {
    auto layout2 = std::make_shared<MaterialPropertyLayoutBuilt>(*layout);

    for (size_t i = 0; i < layout->properties.size(); i++)
        layout2->properties[i] = new MaterialPropertyBuiltGeneric(*layout->properties[i]);

    return layout2;
}
