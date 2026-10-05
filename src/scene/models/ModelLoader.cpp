/*
 * ModelLoader.cpp
 */

#include "ModelLoader.h"

#include "ObjLoader.h"

#include <filesystem>
#include <iostream>

namespace BulletRender {
namespace scene {

// one reader per format, picked by what the file is named
std::vector<MeshData> ModelLoader::read(const std::string& path)
{
    const std::string extension = std::filesystem::path(path).extension().string();

    if (extension == ".obj" || extension == ".OBJ")
    {
        return ObjLoader::read(path);
    }

    std::cerr << "no loader for model: " << path << '\n';
    return {};
}

std::shared_ptr<Model> ModelLoader::upload(const std::vector<MeshData>& meshes)
{
    if (meshes.empty())
    {
        return nullptr;
    }

    auto model = std::make_shared<Model>();

    for (const MeshData& mesh : meshes)
    {
        model->addMesh(mesh.vertices, mesh.indices);
    }

    return model;
}

} // namespace scene
} // namespace BulletRender
