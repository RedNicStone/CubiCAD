//
// Created by nic on 29/12/2021.
//

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#include "modelloader.h"


std::shared_ptr<ModelLoader> ModelLoader::create(const std::shared_ptr<TextureLibrary> &textureLibrary,
                                                 const std::shared_ptr<MaterialLibrary> &materialLibrary) {
    auto modelLoader = std::make_shared<ModelLoader>();

    modelLoader->config.triangulate = true;
    modelLoader->config.triangulation_method = "earcut";
    modelLoader->config.mtl_search_path = "";
    modelLoader->config.vertex_color = false;

    modelLoader->reader = tinyobj::ObjReader();

    modelLoader->textureLibrary = textureLibrary;
    modelLoader->materialLibrary = materialLibrary;

    return modelLoader;
}

void ModelLoader::loadMaterialProperties(char *property,
                                         const MaterialPropertyBuiltGeneric *materialProperty,
                                         tinyobj::material_t material) {
    switch (hash(materialProperty->attributeName)) {
        case "diffuse"_hash:memcpy(&material.diffuse, property, materialProperty->getSize());
            break;
        case "emission"_hash:memcpy(&material.emission, property, materialProperty->getSize());
            break;
        case "transparency"_hash:memcpy(&material.transmittance, property, materialProperty->getSize());
            break;
        case "specular"_hash:memcpy(&material.specular, property, materialProperty->getSize());
            break;
        case "roughness"_hash:memcpy(&material.roughness, property, materialProperty->getSize());
            break;
        case "metallic"_hash:memcpy(&material.metallic, property, materialProperty->getSize());
            break;
        case "sheen"_hash:memcpy(&material.sheen, property, materialProperty->getSize());
            break;
        case "clear_coat_thickness"_hash:memcpy(&material.clearcoat_thickness, property, materialProperty->getSize());
            break;
        case "clear_coat_roughness"_hash:memcpy(&material.clearcoat_roughness, property, materialProperty->getSize());
            break;
        case "anisotropy"_hash:memcpy(&material.anisotropy, property, materialProperty->getSize());
            break;
        case "anisotropy_rotation"_hash:memcpy(&material.anisotropy_rotation, property, materialProperty->getSize());
            break;
    }
}

std::vector<std::shared_ptr<Texture>> ModelLoader::loadMaterialTextures(const MaterialPropertyLayoutBuilt &materialLayout,
                                                                        const tinyobj::material_t &material,
                                                                        const std::shared_ptr<TextureLibrary> &textureLibrary,
                                                                        const std::string& modelFilename) {
    auto textures = std::vector<std::shared_ptr<Texture>>(materialLayout.properties.size(), nullptr);
    for (size_t i = 0; i < materialLayout.properties.size(); i++) {
        std::string texFile;
        switch (hash(materialLayout.properties[i]->attributeName)) {
            case "diffuse"_hash:
                texFile = locateTexture(material.diffuse_texname, modelFilename);
                if (!texFile.empty())
                    textures[i] =
                        textureLibrary->createTexture(texFile,
                                                      materialLayout.properties[i]->pixelFormat);
                break;
            case "emission"_hash:
                texFile = locateTexture(material.emissive_texname, modelFilename);
                if (!texFile.empty())
                    textures[i] =
                        textureLibrary->createTexture(texFile,
                                                      materialLayout.properties[i]->pixelFormat);
                break;
            case "specular"_hash:
                texFile = locateTexture(material.specular_texname, modelFilename);
                if (!texFile.empty())
                    textures[i] =
                        textureLibrary->createTexture(texFile,
                                                      materialLayout.properties[i]->pixelFormat);
                break;
            case "roughness"_hash:
                texFile = locateTexture(material.roughness_texname, modelFilename);
                if (!texFile.empty())
                    textures[i] =
                        textureLibrary->createTexture(texFile,
                                                      materialLayout.properties[i]->pixelFormat);
                break;
            case "metallic"_hash:
                texFile = locateTexture(material.metallic_texname, modelFilename);
                if (!texFile.empty())
                    textures[i] =
                        textureLibrary->createTexture(texFile,
                                                      materialLayout.properties[i]->pixelFormat);
                break;
            case "sheen"_hash:
                texFile = locateTexture(material.sheen_texname, modelFilename);
                if (!texFile.empty())
                    textures[i] =
                        textureLibrary->createTexture(texFile,
                                                      materialLayout.properties[i]->pixelFormat);
                break;
        }
    }

    return textures;
}

std::string ModelLoader::locateTexture(const std::string &textureFilename,
                                       const std::string &modelFilename) {
    if (textureFilename.empty())
        return "";
    auto textureFilenameV = textureFilename;
    Utils::replaceAll(textureFilenameV, "\\", "/");
    auto textureFile = std::filesystem::path(textureFilenameV);
    if (!std::filesystem::exists(textureFile) || std::filesystem::is_directory(textureFile)) {
        auto modelFile = std::filesystem::path(modelFilename);
        textureFile = modelFile.parent_path() / textureFile;
        if (!std::filesystem::exists(textureFile) || std::filesystem::is_directory(textureFile))
            return "";
    }
    return textureFile;
}

std::vector<std::shared_ptr<Mesh>> ModelLoader::import(const std::string &filename,
                                                       const std::shared_ptr<MasterMaterial> &masterMaterial,
                                                       bool normalizePos) {
    if (!reader.ParseFromFile(filename, config)) {
        if (!reader.Error().empty()) {
            throw std::runtime_error(reader.Error());
        }
        throw std::runtime_error("TinyOBJLoader encountered an error");
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();

    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Material>> surfaceMaterials;
    meshes.reserve(shapes.size());
    surfaceMaterials.reserve(materials.size());

    for (const auto &material: materials) {
        char *properties = new char[masterMaterial->getPropertySize()];
        size_t offset = 0;
        for (const auto &property: masterMaterial->getPropertyLayout().properties) {
            loadMaterialProperties(properties + offset, property, material);
            offset += property->getSize();
        }
        auto textures = loadMaterialTextures(masterMaterial->getPropertyLayout(), material, textureLibrary, filename);

        auto mat = materialLibrary->registerShader(masterMaterial, properties, textures, material.name);
        surfaceMaterials.push_back(mat);
    }

    // Loop over shapes
    for (const auto &shape: shapes) {
        std::unordered_map<int, std::shared_ptr<Meshlet>> meshlets;

        BoundingBox bbox{};
        bbox.pos1 = *reinterpret_cast<const glm::vec3 *>(attrib.vertices.data());
        bbox.pos2 = *reinterpret_cast<const glm::vec3 *>(attrib.vertices.data());

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        uint32_t vertexIndex = 0;

        // Loop over faces(polygon)
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

            int materialID = shape.mesh.material_ids[f];
            if (meshlets[materialID] == nullptr) {
                meshlets[materialID] = std::make_shared<Meshlet>();
                if (materialID >= 0)
                    meshlets[materialID]->material = surfaceMaterials[static_cast<unsigned long>(materialID)];
            }

            // Loop over vertices in the face.
            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[f * 3 + v];

                Vertex vertex{};
                vertex.pos =
                    *reinterpret_cast<const glm::vec3 *>(attrib.vertices.data() + 3 * size_t(idx.vertex_index));
                if (idx.texcoord_index >= 0) {
                    vertex.uv =
                        {(attrib.texcoords[static_cast<uint>(2 * idx.texcoord_index + 0)]) * UINT16_MAX,
                         (1.0f - attrib.texcoords[static_cast<uint>(2 * idx.texcoord_index + 1)]) * UINT16_MAX};
                }

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertexIndex);
                    meshlets[materialID]->indexData.push_back(static_cast<uint32_t>(
                                                                  vertexIndex));
                    vertexIndex++;

                    bbox.pos1 = glm::min(bbox.pos1, vertex.pos);
                    bbox.pos2 = glm::max(bbox.pos1, vertex.pos);
                } else
                    meshlets[materialID]->indexData.push_back(uniqueVertices[vertex]);
            }
        }

        std::vector<std::shared_ptr<Meshlet>> meshletVector;
        meshletVector.reserve(meshlets.size());
        for (const auto& kv : meshlets) {
            meshletVector.push_back(kv.second);
        }

        std::vector<Vertex> vertexVector;
        vertexVector.resize(uniqueVertices.size());
        for (const auto& kv : uniqueVertices) {
            vertexVector[kv.second] = kv.first;
        }

        meshes.push_back(Mesh::create(meshletVector, vertexVector, bbox, shape.name, normalizePos));
    }

    return meshes;
}
