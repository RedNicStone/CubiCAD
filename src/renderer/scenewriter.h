//
// Created by nic on 26/01/2022.
//

#pragma once

#ifndef CUBICAD_SCENEWRITER_H
#define CUBICAD_SCENEWRITER_H

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <fstream>

#include "rendermanager.h"


class RenderManager;

struct FileStream {
    std::ofstream charStream;

    void Put(char c) { charStream.put(c); }

    void Flush() {}
};

class SceneWriter {
  private:
    rapidjson::Document document;
    std::shared_ptr<RenderManager> renderManager;

    std::string filename;

    void readInstanceFromNode(const rapidjson::Value &value,
                              std::unordered_map<std::string, std::shared_ptr<Mesh>> &meshMap);
    rapidjson::Value writeInstanceToNode(const std::shared_ptr<MeshInstance> &instance);

    void readMeshesFromNode(const rapidjson::Value &value,
                            std::unordered_map<std::string, std::shared_ptr<Mesh>> &meshMap);
    rapidjson::Value writeMeshesToNode(const std::pair<std::string, std::vector<std::shared_ptr<Mesh>>> &mesh);

    void readSceneFromDocument();
    void writeSceneToDocument();

  public:
    static std::shared_ptr<SceneWriter> create(const std::shared_ptr<RenderManager> &renderManager,
                                               const std::string &filename);

    std::shared_ptr<Material> material;

    void readScene();
    void writeScene();

    void setFilename(const std::string &newFilename) { filename = newFilename; }

    std::string getFilename() { return filename; }
};

#endif //CUBICAD_SCENEWRITER_H
