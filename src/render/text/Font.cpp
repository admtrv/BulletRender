/*
 * Font.cpp
 */

#include "Font.h"

#include <stb_truetype.h>

#include <cmath>
#include <iostream>

namespace BulletRender {
namespace render {

constexpr int ATLAS_SIZE = 1024;
constexpr int GLYPH_PADDING = 1;        // reserved but left outside uv, so linear filter never reaches neighbour

struct Font::Info {
    stbtt_fontinfo handle{};
};

Font::Font(std::vector<unsigned char> data) : m_data(std::move(data))
{
    if (m_data.empty())
    {
        return;
    }

    m_info = std::make_unique<Info>();

    if (!stbtt_InitFont(&m_info->handle, m_data.data(), stbtt_GetFontOffsetForIndex(m_data.data(), 0)))
    {
        m_data.clear();
        return;
    }

    // one channel, glyphs are coverage rather than color
    Texture2DConfig config;
    config.internalFormat = GL_R8;
    config.sampler.minFilter = GL_LINEAR;
    config.sampler.magFilter = GL_LINEAR;
    config.sampler.wrapS = GL_CLAMP_TO_EDGE;
    config.sampler.wrapT = GL_CLAMP_TO_EDGE;

    m_atlas = std::make_unique<Texture2D>(ATLAS_SIZE, ATLAS_SIZE, config);

    // zeroed once, so room no glyph took yet stays transparent
    const std::vector<unsigned char> blank(size_t(ATLAS_SIZE) * ATLAS_SIZE, 0);
    m_atlas->uploadPixels(blank.data(), GL_RED, GL_UNSIGNED_BYTE);
}

Font::~Font() = default;

FontMetrics Font::getMetrics(float size) const
{
    if (!isLoaded())
    {
        return {};
    }

    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
    stbtt_GetFontVMetrics(&m_info->handle, &ascent, &descent, &lineGap);

    const float scale = stbtt_ScaleForPixelHeight(&m_info->handle, size);

    return {ascent * scale, descent * scale, (ascent - descent + lineGap) * scale};
}

float Font::getKerning(uint32_t left, uint32_t right, float size) const
{
    if (!isLoaded())
    {
        return 0.0f;
    }

    const float scale = stbtt_ScaleForPixelHeight(&m_info->handle, size);

    return stbtt_GetCodepointKernAdvance(&m_info->handle, int(left), int(right)) * scale;
}

bool Font::reserve(int width, int height, glm::ivec2& out)
{
    // row is full, drop to next one
    if (m_penPosition.x + width > ATLAS_SIZE)
    {
        m_penPosition.x = 0;
        m_penPosition.y += m_rowHeight;
        m_rowHeight = 0;
    }

    if (m_penPosition.y + height > ATLAS_SIZE)
    {
        return false;
    }

    out = m_penPosition;

    m_penPosition.x += width;
    m_rowHeight = std::max(m_rowHeight, height);

    return true;
}

const Glyph* Font::getGlyph(uint32_t codepoint, float size)
{
    if (!isLoaded())
    {
        return nullptr;
    }

    const Key key{codepoint, uint32_t(std::lround(size))};

    if (auto found = m_glyphs.find(key); found != m_glyphs.end())
    {
        return &found->second;
    }

    const float scale = stbtt_ScaleForPixelHeight(&m_info->handle, size);

    int advance = 0;
    stbtt_GetCodepointHMetrics(&m_info->handle, int(codepoint), &advance, nullptr);

    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    stbtt_GetCodepointBitmapBox(&m_info->handle, int(codepoint), scale, scale, &x0, &y0, &x1, &y1);

    Glyph glyph;
    glyph.advance = advance * scale;
    glyph.bearing = {float(x0), float(y0)};
    glyph.size = {float(x1 - x0), float(y1 - y0)};

    // space carries no picture, only advance
    if (glyph.size.x > 0.0f && glyph.size.y > 0.0f)
    {
        glm::ivec2 corner{0, 0};
        const int width = int(glyph.size.x);
        const int height = int(glyph.size.y);

        // atlas has no room left, glyph is remembered blank so it is asked for once
        if (!reserve(width + GLYPH_PADDING, height + GLYPH_PADDING, corner))
        {
            std::cerr << "font atlas is full, glyph " << codepoint << " dropped\n";

            glyph.size = {0.0f, 0.0f};
            return &m_glyphs.emplace(key, glyph).first->second;
        }

        std::vector<unsigned char> coverage(size_t(width) * height, 0);
        stbtt_MakeCodepointBitmap(&m_info->handle, coverage.data(), width, height, width, scale, scale, int(codepoint));

        m_atlas->uploadSubPixels(corner.x, corner.y, width, height, coverage.data(), GL_RED, GL_UNSIGNED_BYTE);

        glyph.uvMin = glm::vec2(corner) / float(ATLAS_SIZE);
        glyph.uvMax = glm::vec2(corner + glm::ivec2(width, height)) / float(ATLAS_SIZE);
    }

    return &m_glyphs.emplace(key, glyph).first->second;
}

} // namespace render
} // namespace BulletRender
