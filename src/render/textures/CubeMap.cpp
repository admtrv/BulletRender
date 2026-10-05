/*
 * CubeMap.cpp
 */

#include "CubeMap.h"

namespace BulletRender {
namespace render {

static GLenum applySrgb(GLenum internalFormat, bool sRGB)
{
    if (!sRGB)
    {
        return internalFormat;
    }
    switch (internalFormat)
    {
        case GL_RGB8:  return GL_SRGB8;
        case GL_RGBA8: return GL_SRGB8_ALPHA8;
        default:       return internalFormat;
    }
}

CubeMap::CubeMap(int faceSize, const CubeMapConfig& cfg) : Texture(GL_TEXTURE_CUBE_MAP), m_faceSize(faceSize)
{
    m_internalFormat = applySrgb(cfg.internalFormat, cfg.sRGB);

    applySampler(cfg.sampler);
}

void CubeMap::uploadFace(int face, const void* pixels)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
                 static_cast<GLint>(m_internalFormat),
                 m_faceSize, m_faceSize, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

} // namespace render
} // namespace BulletRender
