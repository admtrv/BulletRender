/*
 * Grid.h
 */

#pragma once

#include "RenderPass.h"
#include "Shader.h"

#include <memory>
#include <glad/glad.h>

#define VERT_PATH "assets/shaders/grid.vert.glsl"
#define FRAG_PATH "assets/shaders/grid.frag.glsl"

namespace luchrender {
namespace render {

class Grid final : public IRenderPass {
public:
    explicit Grid(bool enabled = true);
    ~Grid();

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    void render(const scene::Scene& scene, const glm::mat4& view, const glm::mat4& proj) override;

private:
    std::shared_ptr<Shader> m_prog;
    bool m_enabled = true;
    GLuint m_Vao = 0;
};

} // namespace render
} // namespace luchrender
