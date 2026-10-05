/*
 * DepthFrameBuffer.h
 */

#pragma once

#include <glad/glad.h>

#include <string>

namespace BulletRender {
namespace render {

// depth-only FBO used as a shadow map target
class DepthFrameBuffer {
public:
    DepthFrameBuffer(int width, int height, std::string name = {});
    ~DepthFrameBuffer();

    DepthFrameBuffer(const DepthFrameBuffer&) = delete;
    DepthFrameBuffer& operator=(const DepthFrameBuffer&) = delete;

    void bind();
    void unbind();

    GLuint getId() const { return m_fbo; }
    GLuint getDepthTexture() const { return m_depthTex; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    void create();
    void destroy();

    GLuint m_fbo = 0;
    GLuint m_depthTex = 0;

    std::string m_name;
    int m_width = 0;
    int m_height = 0;
};

} // namespace render
} // namespace BulletRender
