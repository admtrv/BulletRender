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

private:
    std::shared_ptr<GraphicsShader> m_prog;
    GLuint m_Vao = 0;
};

} // namespace render
} // namespace BulletRender
