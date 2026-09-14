/*
 * FrameBuffer.h
 */

#pragma once

#include <glad/glad.h>

#include <iostream>

namespace BulletRender {
namespace render {

class FrameBuffer {
public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    void bind();
    void unbind();
    void resize(int width, int height);

    // where unbind returns to, zero is the screen
    static void setDefaultTarget(unsigned target) { s_defaultTarget = target; }

    // handles
    GLuint getId() const { return m_fbo; }
    GLuint getColorTexture() const { return m_colorTex; }
    GLuint getDepthTexture() const { return m_depthTex; }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    void create();
    void destroy();

    GLuint m_fbo = 0;
    GLuint m_colorTex = 0;
    GLuint m_depthTex = 0;

    int m_width = 0;
    int m_height = 0;

    static unsigned s_defaultTarget;
};

} // namespace render
} // namespace BulletRender
