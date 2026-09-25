/*
 * Grid.h
 */

#pragma once

#include "RenderPass.h"
#include "render/Shader.h"

#include <memory>
#include <glad/glad.h>

namespace BulletRender {
namespace render {

#define VERT_PATH "assets/shaders/grid.vert.glsl"
#define FRAG_PATH "assets/shaders/grid.frag.glsl"

class Grid final : public IRenderPass {
public:
    explicit Grid(bool enabled = true);
    ~Grid();

    void render(const scene::Scene& scene) override;

    float getFadeStart() const { return m_fadeStart; }
    float getFadeEnd() const { return m_fadeEnd; }
    void setFade(float start, float end) { m_fadeStart = start; m_fadeEnd = end; }

private:
    std::shared_ptr<GraphicsShader> m_prog;
    GLuint m_Vao = 0;

    float m_fadeStart = 0.5f;
    float m_fadeEnd = 0.95f;
};

} // namespace render
} // namespace BulletRender
