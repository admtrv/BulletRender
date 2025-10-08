/*
 * Light.h
 */

#pragma once

#include <glm/glm.hpp>

namespace luchrender {
namespace scene {

// basic light
class Light {
public:
    virtual ~Light() = default;

    virtual glm::vec3 getDirection() const = 0;
};

// directional light
class DirectionalLight : public Light {
public:
    explicit DirectionalLight(glm::vec3 dir = glm::normalize(glm::vec3(-0.4f, 1.0f, 0.2f)))
        : m_direction(glm::normalize(dir)) {}

    glm::vec3 getDirection() const override { return m_direction; }

private:
    glm::vec3 m_direction;
};

} // namespace scene
} // namespace luchrender