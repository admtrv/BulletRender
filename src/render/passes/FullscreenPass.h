/*
 * FullscreenPass.h
 */

#pragma once

#include "RenderPass.h"

#include <glad/glad.h>

namespace BulletRender {
namespace render {

// abstract base for any pass that covers the screen with one triangle
class FullscreenPass : public IRenderPass {
public:
    ~FullscreenPass() override;

protected:
    FullscreenPass();

    // single oversized triangle, no vbo, uses gl_VertexID in the vertex shader
    void drawFullscreenTriangle();

    GLuint m_vao = 0;
};

} // namespace render
} // namespace BulletRender
