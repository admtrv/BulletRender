/*
 * Material.h
 */

#pragma once

#include "Shader.h"

#include <glm/glm.hpp>

#include <memory>

namespace luchrender {
namespace render {

// simple material (color)
class Material {
public:
    void setShader(std::shared_ptr<Shader> s) noexcept { m_shader = std::move(s); }
    std::shared_ptr<Shader> getShader() const noexcept { return m_shader; }

    void setColor(const glm::vec3& c) noexcept { m_color = c; }
    const glm::vec3& getColor() const noexcept { return m_color; }

private:
    std::shared_ptr<Shader> m_shader;
    glm::vec3 m_color{1.0f, 1.0f, 1.0f};

};

} // namespace render
} // namespace luchrender