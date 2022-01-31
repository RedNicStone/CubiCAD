//
// Created by nic on 26/01/2022.
//

#include "scenewriter.h"


std::shared_ptr<SceneWriter> SceneWriter::create(const std::shared_ptr<RenderManager>& renderManager,
                                                 const std::string& filename) {
    auto sceneWriter = std::make_shared<SceneWriter>();
    sceneWriter->renderManager = renderManager;
    sceneWriter->filename = filename;

    return sceneWriter;
}

void SceneWriter::readInstanceFromNode(const rapidjson::Value& value) {
    if (!value.HasMember("name") || !value.HasMember("mesh")
     || !value.HasMember("pos")  || !value.HasMember("rot")  || !value.HasMember("scale")) {
        std::cerr << "Instance is missing a field, skipping" << std::endl;
        return;
    }
    if (!value["name"].IsString() || !value["mesh"].IsString()
     || !value["pos"].IsArray()   || !value["rot"].IsArray()   || !value["scale"].IsArray()) {
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

    renderManager->getScene()->submitInstance(instance);
}

rapidjson::Value SceneWriter::writeInstanceToNode(const std::shared_ptr<MeshInstance>& instance) {
    rapidjson::Value node(rapidjson::kObjectType);

    node.AddMember("name", instance->getName(), document.GetAllocator());
    node.AddMember("mesh", instance->getMesh()->getName(), document.GetAllocator());

    rapidjson::Value pos(rapidjson::kArrayType);
    rapidjson::Value rot(rapidjson::kArrayType);
    rapidjson::Value scale(rapidjson::kArrayType);

    for (int i = 0; i < 3; i++) {
        pos.PushBack(instance->getPosition()[i], document.GetAllocator());
        rot.PushBack(instance->getRotation()[i], document.GetAllocator());
        scale.PushBack(instance->getScale()[i], document.GetAllocator());
    }

    node.AddMember("pos", pos.Move(), document.GetAllocator());
    node.AddMember("rot", rot.Move(), document.GetAllocator());
    node.AddMember("scale", scale.Move(), document.GetAllocator());

    return node;
}

void SceneWriter::readMeshesFromNode(const rapidjson::Value& value,
                                     const std::unordered_map<std::string, std::vector<std::shared_ptr<Mesh>>>&
                                         meshMap) {
    if (!value.HasMember("file") || !value.HasMember("meshes")) {
        std::cerr << "MeshFiles is missing a field, skipping" << std::endl;
        return;
    }
    if (!value["file"].IsString() || !value["meshes"].IsArray()) {
        std::cerr << "MeshFiles field invalid, skipping" << std::endl;
        return;
    }
    auto meshNodes = value["meshes"].GetArray();
    auto meshes = renderManager->getMeshLibrary()->createMesh(value["file"].GetString(), material);
    for (uint x = 0; x < meshNodes.Size(); x++) {
        if (!meshNodes[x].HasMember("name") || !meshNodes[x].HasMember("materials")) {
            std::cerr << "Mesh is missing a field, skipping" << std::endl;
            return;
        }
        if (!meshNodes[x]["name"].IsString() || !meshNodes[x]["materials"].IsArray()) {
            std::cerr << "Mesh field invalid, skipping" << std::endl;
            return;
        }
        meshes[x]->setName(meshNodes[x]["name"].GetString());
        for (uint y = 0; y < meshNodes[y]["materials"].Size(); y++) {
            // do something pls
            //todo add material loading from save
        }
        meshMap[meshes[x]->getName()] = meshes[x];
    }
}

rapidjson::Value SceneWriter::writeMeshesToNode(const std::pair<std::string, std::vector<std::shared_ptr<Mesh>>>&
    pair) {
    rapidjson::Value node(rapidjson::kObjectType);
    node.AddMember("file", pair.first, document.GetAllocator());

    rapidjson::Value meshes(rapidjson::kArrayType);
    for (const auto& mesh : pair.second) {
        rapidjson::Value meshNode(rapidjson::kObjectType);
        meshNode.AddMember("file", filename, document.GetAllocator());

        rapidjson::Value meshletArray(rapidjson::kArrayType);
        for (const auto& meshlet : mesh->getMeshlets()) {
            meshletArray.PushBack(rapidjson::Value().SetString(meshlet->material->getName(), document.GetAllocator()),
                                  document.GetAllocator());
        }

        meshNode.AddMember("materials", meshletArray.Move(), document.GetAllocator());

        meshes.PushBack(meshNode.Move(), document.GetAllocator());
    }

    node.AddMember("meshes", meshes.Move(), document.GetAllocator());
    return node;
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

void SceneWriter::writeSceneToDocument() {
    document.SetObject();

    rapidjson::Value objectArray(rapidjson::kArrayType);
    for (const auto& instance : renderManager->getScene()->getInstances())
        objectArray.PushBack(writeInstanceToNode(instance).Move(), document.GetAllocator());
    document.AddMember("objects", objectArray, document.GetAllocator());

    rapidjson::Value meshArray(rapidjson::kArrayType);
    for (const auto& meshes : renderManager->getMeshLibrary()->getMeshes())
        meshArray.PushBack(writeMeshesToNode(meshes).Move(), document.GetAllocator());
    document.AddMember("meshFiles", meshArray, document.GetAllocator());
}

void SceneWriter::readScene() {
    std::ifstream fileStream(filename, std::ifstream::in);
    if (!fileStream.good()) {
        std::cerr << "Unable to open file \"" << filename << "\" in read mode" << std::endl;
        fileStream.close();
        return;
    }

    rapidjson::BasicIStreamWrapper<std::istream> stream(fileStream);
    document.ParseStream(stream);

    fileStream.close();
}

void SceneWriter::writeScene() {
    std::ofstream fileStream(filename, std::ofstream::out | std::ofstream::trunc);
    if (!fileStream.good()) {
        std::cerr << "Unable to open file \"" << filename << "\" in write mode" << std::endl;
        fileStream.close();
        return;
    }

    rapidjson::BasicOStreamWrapper<std::ostream> stream(fileStream);
    rapidjson::PrettyWriter<rapidjson::BasicOStreamWrapper<std::ostream>> writer(stream);
    writer.SetMaxDecimalPlaces(6);
    writeSceneToDocument();
    document.Accept(writer);

    fileStream.close();
}
