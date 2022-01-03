//
// Created by nic on 29/12/2021.
//

#include "modelloader.h"


std::shared_ptr<ModelLoader> ModelLoader::create() {
    auto modelLoader = std::make_shared<ModelLoader>();

    modelLoader->config.triangulate = true;
    modelLoader->config.triangulation_method = "earcut";
}

std::shared_ptr<Mesh> ModelLoader::import(const std::string& filename) {
    if(!reader.ParseFromFile(filename))
}
