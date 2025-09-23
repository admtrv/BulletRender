/*
 * material.h
 */
#pragma once
#include <glm/glm.hpp>

namespace luchrender {
namespace render {

// simple material (color)
class Material {
public:
    void setColor(const glm::vec3& c) noexcept { m_color = c; }
    const glm::vec3& getColor() const noexcept { return m_color; }

private:
    glm::vec3 m_color{1.0f, 1.0f, 1.0f};

};

} // namespace render
} // namespace luchrender