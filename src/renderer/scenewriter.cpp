//
// Created by nic on 26/01/2022.
//

#include "scenewriter.h"


std::shared_ptr<SceneWriter> SceneWriter::create(const std::shared_ptr<Scene>& scene, const std::string& filename) {
    auto sceneWriter = std::make_shared<SceneWriter>();
    sceneWriter->scene = scene;
    sceneWriter->filename = filename;

    return sceneWriter;
}

void SceneWriter::readInstanceFromNode(const rapidjson::Value& value) {
    if (!value.HasMember("name") || !value.HasMember("id") || !value.HasMember("mesh") ||
        !value.HasMember("pos") || !value.HasMember("rot") || !value.HasMember("scale")) {
        std::cerr << "Instance is missing a field, skipping" << std::endl;
        return;
    }
    if (!value["name"].IsString() || !value["id"].IsInt64() || !value["mesh"].IsString() ||
        !value["pos"].IsArray() || !value["rot"].IsArray() || !value["scale"].IsArray()) {
        std::cerr << "Instance field invalid, skipping" << std::endl;
        return;
    }
    auto pos = value["pos"].GetArray();
    auto rot = value["rot"].GetArray();
    auto scale = value["scale"].GetArray();
    for (const auto& var : { pos, rot, scale }) {
        if (var.Size() < 3) {
            std::cerr << "Instance vector field has to few arguments, skipping" << std::endl;
            return;
        }
        if (!var[0].IsFloat() || !var[1].IsFloat() || !var[2].IsFloat()) {
            std::cerr << "Instance vector field argument is not float, skipping" << std::endl;
            return;
        }
    }

    auto instance = MeshInstance::create(nullptr, value["name"].GetString());
    instance->setID(static_cast<uint32_t>(value["id"].GetInt64()));
    instance->setPosition({ pos[0].GetFloat(), pos[1].GetFloat(), pos[2].GetFloat() });
    instance->setPosition({ rot[0].GetFloat(), rot[1].GetFloat(), rot[2].GetFloat() });
    instance->setPosition({ scale[0].GetFloat(), scale[1].GetFloat(), scale[2].GetFloat() });

    scene->submitInstance(instance);
}

void SceneWriter::readSceneFromDocument() {
    if (!document.IsObject()) {
        std::cerr << "File is not a valid CubiCAD save file" << std::endl;
        return;
    }
    if (!document.HasMember("materials")) {
        std::cerr << "File does not contain any materials" << std::endl;
    } else if (!document["materials"].IsArray()) {
        std::cerr << "File's material list has been corrupted" << std::endl;
    } else {
        for (const auto& instance : document["materials"].GetArray()) {
            readInstanceFromNode(instance);
        }
    }
    if (!document.HasMember("meshes")) {
        std::cerr << "File does not contain any meshes" << std::endl;
    } else if (!document["meshes"].IsArray()) {
        std::cerr << "File's mesh list has been corrupted" << std::endl;
    } else {
        for (const auto& instance : document["meshes"].GetArray()) {
            readInstanceFromNode(instance);
        }
    }
    if (!document.HasMember("instances")) {
        std::cerr << "File does not contain any instances" << std::endl;
    } else if (!document["instances"].IsArray()) {
        std::cerr << "File's instance list has been corrupted" << std::endl;
    } else {
        for (const auto& instance : document["instances"].GetArray()) {
            readInstanceFromNode(instance);
        }
    }
}

void SceneWriter::readScene() {
    std::FILE* file = fopen(filename.c_str(), "r");
    fseek(file, 0L, SEEK_END);
    auto size = static_cast<unsigned long>(ftell(file));
    rewind(file);
    char* data = new char[size];

    rapidjson::FileReadStream steam(file, data, size);
    document.ParseStream(steam);

    fclose(file);


}
