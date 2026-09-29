/*
 * Material.h
 */

#pragma once

#include "Shader.h"
#include "textures/TextureSlot.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

namespace BulletRender {
namespace render {

enum class Shading : uint8_t {
    Lit,
    Unlit       // shown as it is, what a flat picture expects
};

enum class AlphaMode : uint8_t {
    Opaque,     // ignored
    Mask,       // pixel is kept whole or dropped whole
    Blend       // drawn after solid ones and mixed with them
};

class Material {
public:
    std::shared_ptr<GraphicsShader> shader;

    Shading shading = Shading::Lit;

    // base
    glm::vec3 diffuse{1.0f};
    TextureSlot diffuseTexture;

    // specular, ks and ns of phong
    glm::vec3 specular{0.5f};
    TextureSlot specularTexture;
    float shininess = 32.0f;

    // detail
    TextureSlot normalTexture;

    // light it gives off on its own
    glm::vec3 emissive{0.0f};
    TextureSlot emissiveTexture;

    // rendering
    AlphaMode alphaMode = AlphaMode::Opaque;
    float alphaCutoff = 0.5f;
    bool doubleSided = false;

    bool isBlended() const { return alphaMode == AlphaMode::Blend; }
};

} // namespace render
} // namespace BulletRender
