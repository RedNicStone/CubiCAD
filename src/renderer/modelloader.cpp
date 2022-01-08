//
// Created by nic on 29/12/2021.
//

#include "modelloader.h"


std::shared_ptr<ModelLoader> ModelLoader::create() {
    auto modelLoader = std::make_shared<ModelLoader>();

    modelLoader->config.triangulate = true;
    modelLoader->config.triangulation_method = "earcut";
    modelLoader->config.mtl_search_path = "./";
    modelLoader->config.vertex_color = false;
}

std::shared_ptr<Mesh> ModelLoader::import(std::shared_ptr<MasterMaterial> masterMaterial, const std::string& filename) {
    if(!reader.ParseFromFile(filename)) {
        if (!reader.Error().empty()) {
            throw std::runtime_error(reader.Error());
        }
        throw std::runtime_error("TinyOBJLoader encountered an error");
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& rAttrib = reader.GetAttrib();
    auto& rShapes = reader.GetShapes();
    auto& rMaterials = reader.GetMaterials();

    std::vector<std::shared_ptr<Material>> materials(rMaterials.size());
    for (size_t iMaterial = 0; iMaterial < rMaterials.size(); iMaterial++) {
        materials[iMaterial] = masterMaterial
    }

    std::vector<std::shared_ptr<Mesh>> meshes;
    meshes.reserve(rShapes.size());

    // Loop over shapes
    for (const auto & shape : rShapes) {
        // Loop over faces(polygon)

        std::unordered_map<int, std::shared_ptr<Meshlet>> meshlets;

        size_t index_offset = 0;
        for (size_t face = 0; face < shape.mesh.num_face_vertices.size(); face++) {

            // Loop over vertices in the face.
            for (size_t vertex = 0; vertex < 3; vertex++) {
                // access to vertex
                tinyobj::index_t idx = shape.mesh.indices[index_offset + vertex];
                tinyobj::real_t vx = rAttrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = rAttrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = rAttrib.vertices[3*size_t(idx.vertex_index)+2];

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = rAttrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = rAttrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = rAttrib.normals[3*size_t(idx.normal_index)+2];
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = rAttrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = rAttrib.texcoords[2*size_t(idx.texcoord_index)+1];
                }

                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
            }
            index_offset += 3;

            // per-face material
            shape.mesh.material_ids[face];
        }

        meshes.push_back(std::make_shared<Mesh>());
    }
}
