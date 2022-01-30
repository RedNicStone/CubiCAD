//
// Created by nic on 26/01/2022.
//

#ifndef CUBICAD_SCENEWRITER_H
#define CUBICAD_SCENEWRITER_H

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <fstream>

#include "scene.h"


struct FileStream {
    std::ofstream charStream;
    void Put (char c) {charStream.put (c);}
    void Flush() {}
};

class SceneWriter {
  private:
    rapidjson::Document document;
    std::shared_ptr<Scene> scene;

    std::string filename;

    void readInstanceFromNode(const rapidjson::Value& value);
    rapidjson::Value writeInstanceToNode(const std::shared_ptr<MeshInstance>& instance);

    void readSceneFromDocument();
    void writeSceneToDocument();

  public:
    static std::shared_ptr<SceneWriter> create(const std::shared_ptr<Scene>& scene, const std::string& filename);

    void readScene();
    void writeScene();
};

#endif //CUBICAD_SCENEWRITER_H
