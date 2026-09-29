/*
 * ModelLoader.cpp
 */

#include "ModelLoader.h"

#include "ObjLoader.h"

#include <filesystem>
#include <iostream>

namespace BulletRender {
namespace scene {

ModelLoader& ModelLoader::instance()
{
    static ModelLoader inst;
    return inst;
}

std::shared_ptr<Model> ModelLoader::load(const std::string& path)
{
    auto it = m_cache.find(path);
    if (it != m_cache.end())
    {
        if (auto cached = it->second.lock())
        {
            return cached;
        }
        m_cache.erase(it);
    }

    auto model = loadFromDisk(path);
    if (model)
    {
        m_cache[path] = model;
    }
    return model;
}

void ModelLoader::remove(const std::string& path)
{
    m_cache.erase(path);
}

void ModelLoader::clear()
{
    m_cache.clear();
}

// one loader per format, picked by what the file is named
std::shared_ptr<Model> ModelLoader::loadFromDisk(const std::string& path)
{
    const std::string extension = std::filesystem::path(path).extension().string();

    if (extension == ".obj" || extension == ".OBJ")
    {
        return ObjLoader::load(path);
    }

    std::cerr << "no loader for model: " << path << '\n';
    return nullptr;
}

} // namespace scene
} // namespace BulletRender
