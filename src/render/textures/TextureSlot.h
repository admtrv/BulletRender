/*
 * TextureSlot.h
 */

#pragma once

#include "Texture2D.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

namespace BulletRender {
namespace render {

// how a texture is read when it does not land pixel for pixel on screen
enum class TextureFilter : uint8_t {
    Smooth,     // blended, what a photograph wants
    Pixel       // nearest texel, what pixel art wants
};

// what happens past the edge of the picture, seen once uv leaves 0..1
enum class TextureWrap : uint8_t {
    Repeat,
    Clamp,
    Mirror
};

// reading settings, on slot so one picture may be read two ways by two materials
struct Sampler {
    TextureFilter filter = TextureFilter::Smooth;
    TextureWrap wrapU = TextureWrap::Repeat;
    TextureWrap wrapV = TextureWrap::Repeat;

    // image files run top down while uv climbs, uv may already account for that
    bool flipU = false;
    bool flipV = false;
};

// one picture a material samples, with how to read it and what part to take
struct TextureSlot {
    std::shared_ptr<Texture2D> texture;

    Sampler sampler;

    // window into the picture, whole of it by default, one frame of a sheet otherwise
    glm::vec2 uvOffset{0.0f};
    glm::vec2 uvScale{1.0f};

    bool empty() const { return texture == nullptr; }
};

} // namespace render
} // namespace BulletRender
