/*
 * ModelLoader.h
 */

#pragma once

#include "Model.h"

#include <memory>
#include <string>
#include <vector>

namespace BulletRender {
namespace scene {

// one mesh, before gl has buffer for it
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};

class ModelLoader {
public:
    static std::vector<MeshData> read(const std::string& path);
    static std::shared_ptr<Model> upload(const std::vector<MeshData>& meshes, const std::string& path = {});
};

} // namespace scene
} // namespace BulletRender
