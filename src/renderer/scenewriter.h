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

/// File steam structure used to ease in creation of buffered file steams
struct FileStream {
    std::ofstream charStream;

    /// Append a new character to the steam
    /// \param c Character to append
    void Put(char c) { charStream.put(c); }

    /// Flush all character to file
    void Flush() {}
};

/// Scene writer class used for reading and writing to save files
class SceneWriter {
  private:
    rapidjson::Document document;
    std::shared_ptr<RenderManager> renderManager;

    std::string filename;

    /// Read instance data from a json node
    /// \param value Node to read from
    /// \param meshMap Map to append the node to
    void readInstanceFromNode(const rapidjson::Value &value,
                              std::unordered_map<std::string, std::shared_ptr<Mesh>> &meshMap);
    /// Write instance to json node
    /// \param instance Instance to write
    /// \return JSON node with instance data
    rapidjson::Value writeInstanceToNode(const std::shared_ptr<MeshInstance> &instance);

    /// Read mesh data from a json node
    /// \param value Node to read from
    /// \param meshMap Map of meshes to append the mesh to
    void readMeshesFromNode(const rapidjson::Value &value,
                            std::unordered_map<std::string, std::shared_ptr<Mesh>> &meshMap);
    /// Write mesh data to a json node
    /// \param mesh Mesh to write
    /// \return JSON node with mesh data
    rapidjson::Value writeMeshesToNode(const std::pair<std::string, std::vector<std::shared_ptr<Mesh>>> &mesh);

    /// Read a whole scene from current document
    void readSceneFromDocument();
    /// Write a whole scene to current document
    void writeSceneToDocument();

  public:
    /// Create a new SceneWriter instance
    /// \param renderManager RenderManager handle to use for all operations
    /// \param filename Default file used for reading and writing scenes
    /// \return Valid handle to SceneWriter
    static std::shared_ptr<SceneWriter> create(const std::shared_ptr<RenderManager> &renderManager,
                                               const std::string &filename);

    std::shared_ptr<Material> material;

    /// Read scene from current file
    void readScene();
    /// Write scene to current file
    void writeScene();

    /// Set current file
    /// \param newFilename Filename of file to be used for reading adn writing actions
    void setFilename(const std::string &newFilename) { filename = newFilename; }

    std::string getFilename() { return filename; }
};

#endif //CUBICAD_SCENEWRITER_H
