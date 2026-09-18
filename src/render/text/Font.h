/*
 * Font.h
 */

#pragma once

#include "render/textures/Texture2D.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace BulletRender {
namespace render {

// where glyph sits in atlas, and how pen moves past it
struct Glyph {
    glm::vec2 uvMin{0.0f};
    glm::vec2 uvMax{0.0f};

    glm::vec2 size{0.0f};       // drawn box, pixels
    glm::vec2 bearing{0.0f};    // from pen to box corner
    float advance = 0.0f;       // to next pen position
};

// how far line reaches around its baseline, and to next one
struct FontMetrics {
    float ascent = 0.0f;
    float descent = 0.0f;
    float lineHeight = 0.0f;
};

// typeface over ttf data, rasterized on demand into one atlas
//
// glyphs are kept per size, since 16 and 32 px are different pictures
class Font {
public:
    explicit Font(std::vector<unsigned char> data);
    ~Font();

    bool isLoaded() const { return !m_data.empty(); }

    // rasterizes glyph on first ask, atlas grows row by row
    const Glyph* getGlyph(uint32_t codepoint, float size);
    FontMetrics getMetrics(float size) const;

    // how far pen moves when one glyph follows another
    float getKerning(uint32_t left, uint32_t right, float size) const;

    const Texture2D* getAtlas() const { return m_atlas.get(); }

private:
    // same glyph at another size is another picture, so size is part of key
    struct Key {
        uint32_t codepoint;
        uint32_t size;      // rounded pixels, fractions would never hit cache

        bool operator==(const Key& other) const { return codepoint == other.codepoint && size == other.size; }
    };

    struct KeyHash {
        size_t operator()(const Key& key) const { return std::hash<uint64_t>{}((uint64_t(key.size) << 32) | key.codepoint); }
    };

    // finds room for one glyph, filling atlas row by row
    bool reserve(int width, int height, glm::ivec2& out);

    std::vector<unsigned char> m_data;
    std::unique_ptr<Texture2D> m_atlas;

    std::unordered_map<Key, Glyph, KeyHash> m_glyphs;

    // pen walking atlas, rows stack downwards
    glm::ivec2 m_penPosition{0, 0};
    int m_rowHeight = 0;

    // opaque stbtt_fontinfo, kept out of header
    struct Info;
    std::unique_ptr<Info> m_info;
};

} // namespace render
} // namespace BulletRender
