/*
 * WorldAxis.h
 */

#pragma once

#include "RenderPass.h"
#include "render/Shader.h"

#include <memory>
#include <glad/glad.h>

namespace BulletRender {
namespace render {

#define AXIS_VERT_PATH "assets/shaders/axis.vert.glsl"
#define AXIS_FRAG_PATH "assets/shaders/axis.frag.glsl"

class WorldAxis final : public IRenderPass {
public:
    explicit WorldAxis(bool enabled = true);
    ~WorldAxis();

    void render(const scene::Scene& scene) override;

    // set to match grid, both lie in one plane
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
