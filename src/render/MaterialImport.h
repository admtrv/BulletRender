/*
 * MaterialImport.h
 */

#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace BulletRender {
namespace render {

// what .mtl entry says, paths absolute so caller may load them straight away
struct MaterialImport {
    std::string name;

    glm::vec3 diffuse{1.0f};
    glm::vec3 specular{0.5f};
    glm::vec3 emissive{0.0f};
    float shininess = 32.0f;

    std::string diffuseTexture;
    std::string specularTexture;
    std::string normalTexture;
    std::string emissiveTexture;
};

// reads .mtl, caller fills material itself, so no file reaches surface unasked
std::vector<MaterialImport> readMtl(const std::string& path);

} // namespace render
} // namespace BulletRender
