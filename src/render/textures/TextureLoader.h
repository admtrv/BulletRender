/*
 * TextureLoader.h
 */

#pragma once

#include "Texture2D.h"

#include <memory>
#include <string>
#include <vector>

namespace BulletRender {
namespace render {

struct TextureLoadOptions {
    bool sRGB = false;      // pipeline stays in srgb, decoding would darken the texture
    bool generateMipmaps = true;
    bool flipVertically = false;
    SamplerConfig sampler{};
};

// unpacked file, before gl has seen any of it
struct TexturePixels {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> data;
    TextureLoadOptions options;
    std::string path;

    bool empty() const { return data.empty(); }
};

class TextureLoader {
public:
    static TexturePixels read(const std::string& path, const TextureLoadOptions& options = {});
    static std::shared_ptr<Texture2D> upload(const TexturePixels& pixels);
};

} // namespace render
} // namespace BulletRender
