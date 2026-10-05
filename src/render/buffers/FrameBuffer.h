/*
 * FrameBuffer.h
 */

#pragma once

#include <glad/glad.h>

#include <string>

#include <iostream>

namespace BulletRender {
namespace render {

class FrameBuffer {
public:
    FrameBuffer(int width, int height, std::string name = {});
    ~FrameBuffer();

    void bind();
    void unbind();
    void resize(int width, int height);


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

    std::string m_name;     // resize makes new objects, label needs it again
    int m_width = 0;
    int m_height = 0;

};

} // namespace render
} // namespace BulletRender
