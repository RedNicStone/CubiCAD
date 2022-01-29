//
// Created by nic on 26/01/2022.
//

#ifndef CUBICAD_SCENEWRITER_H
#define CUBICAD_SCENEWRITER_H

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "scene.h"


class SceneWriter {
  private:
    rapidjson::Document document;
    std::shared_ptr<Scene> scene;

    std::string filename;

    void readInstanceFromNode(const rapidjson::Value& value);
    void readSceneFromDocument();

  public:
    static std::shared_ptr<SceneWriter> create(const std::shared_ptr<Scene>& scene, const std::string& filename);

    void readScene();
    void writeScene();
};

#endif //CUBICAD_SCENEWRITER_H
