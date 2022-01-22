//
// Created by nic on 29/12/2021.
//

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#include "modelloader.h"


std::shared_ptr<ModelLoader> ModelLoader::create() {
    auto modelLoader = std::make_shared<ModelLoader>();

    modelLoader->config.triangulate = true;
    modelLoader->config.triangulation_method = "earcut";
    modelLoader->config.mtl_search_path = "./";
    modelLoader->config.vertex_color = false;

    modelLoader->reader = tinyobj::ObjReader();

    return modelLoader;
}

std::vector<std::shared_ptr<Mesh>> ModelLoader::import(const std::string& filename, const std::shared_ptr<Material>&
    material) {
    if(!reader.ParseFromFile(filename, config)) {
        if (!reader.Error().empty()) {
            throw std::runtime_error(reader.Error());
        }
        throw std::runtime_error("TinyOBJLoader encountered an error");
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<std::shared_ptr<Mesh>> meshes;
    //std::vector<std::shared_ptr<Material>> surfaceMaterials;
    meshes.reserve(shapes.size());

    /*for (const auto& material : materials) {

    }*/

    // Loop over shapes
    for (const auto& shape : shapes) {
        std::unordered_map<int, std::shared_ptr<Meshlet>> meshlets;

        // Loop over faces(polygon)
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int materialID = shape.mesh.material_ids[f];
            if (meshlets[materialID] == nullptr) {
                meshlets[materialID] = std::make_shared<Meshlet>();
            }

            // Loop over vertices in the face.
            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[f * 3 + v];

                Vertex vertex{};
                vertex.pos = *reinterpret_cast<const glm::vec3*>(attrib.vertices.data() + 3*size_t(idx.vertex_index));
                if (idx.texcoord_index >= 0) {
                    vertex.uv = *reinterpret_cast<const glm::vec2*>(attrib.texcoords.data()
                        + 2*size_t(idx.texcoord_index));
                }
                meshlets[materialID]->vertexData.push_back(vertex);

                meshlets[materialID]->indexData.push_back(static_cast<uint32_t>(meshlets[materialID]->indexData.size
                ()));
            }
        }

        std::vector<std::shared_ptr<Meshlet>> meshletVector;
        meshletVector.reserve(meshlets.size());

        for (const auto& kv : meshlets) {
            kv.second->material = material;
            meshletVector.push_back(kv.second);
        }

        meshes.push_back(Mesh::create(meshletVector, shape.name));
    }

    return meshes;
}
