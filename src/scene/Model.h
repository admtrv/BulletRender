/*
 * Model.h
 */

#pragma once

#include "tiny_obj_loader.h"

#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include <unordered_map>
#include <iostream>

namespace luchrender {
namespace scene {

class Model {
public:
    Model() = default;
    explicit Model(const std::string& path) { loadObj(path); }
    ~Model() { m_meshes.clear(); }

    bool loadObj(const std::string& path);
    const std::vector<Mesh>& getMeshes() const { return m_meshes; }
private:
    std::vector<Mesh> m_meshes;
};

} // namespace scene
} // namespace luchrender