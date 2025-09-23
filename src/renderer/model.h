/*
 * model.h
 */

#pragma once

#include "tiny_obj_loader.h"
#include "mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include <unordered_map>
#include <iostream>

namespace renderer {

class Model {
public:
    bool loadObj(const std::string& path);
    const std::vector<Mesh>& getMeshes() const { return m_meshes; }
private:
    std::vector<Mesh> m_meshes;
};

} // namespace renderer