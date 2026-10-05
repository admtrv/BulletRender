/*
 * CubeMap.h
 */

#pragma once

#include "Texture.h"

namespace BulletRender {
namespace render {

constexpr int CUBE_FACE_COUNT = 6;

struct CubeMapConfig {
    GLenum internalFormat = GL_RGBA8;
    bool sRGB = false;      // pipeline stays in srgb, decoding would darken the sky
    bool flipVertically = false;
    SamplerConfig sampler{
        .minFilter = GL_LINEAR,
        .magFilter = GL_LINEAR,
        .wrapS = GL_CLAMP_TO_EDGE,
        .wrapT = GL_CLAMP_TO_EDGE,
        .wrapR = GL_CLAMP_TO_EDGE,
        .maxAnisotropy = 1.0f,
    };
};

// gl cubemap texture, 6 faces in order: +X, -X, +Y, -Y, +Z, -Z
class CubeMap final : public Texture {
public:
    explicit CubeMap(int faceSize, const CubeMapConfig& cfg = {});

    void uploadFace(int face, const void* pixels);      // index into order above

    // what it was made as
    int faceSize() const { return m_faceSize; }
    GLenum internalFormat() const { return m_internalFormat; }

private:
    int m_faceSize = 0;
    GLenum m_internalFormat = GL_RGBA8;
};

} // namespace render
} // namespace BulletRender
