/*
 * TextureLoader.cpp
 */

#include "TextureLoader.h"

#include <stb_image.h>

#include <iostream>

namespace BulletRender {
namespace render {

constexpr int CHANNELS = 4;

TexturePixels TextureLoader::read(const std::string& path, const TextureLoadOptions& options)
{
    TexturePixels pixels;
    pixels.options = options;

    // readers run side by side, so this must not reach past the one asking
    stbi_set_flip_vertically_on_load_thread(options.flipVertically ? 1 : 0);

    int channels = 0;
    stbi_uc* read = stbi_load(path.c_str(), &pixels.width, &pixels.height, &channels, CHANNELS);

    if (!read)
    {
        std::cerr << "texture read failed: " << path << " (" << stbi_failure_reason() << ")\n";
        return pixels;
    }

    pixels.data.assign(read, read + size_t(pixels.width) * pixels.height * CHANNELS);
    stbi_image_free(read);

    return pixels;
}

std::shared_ptr<Texture2D> TextureLoader::upload(const TexturePixels& pixels)
{
    if (pixels.empty())
    {
        return nullptr;
    }

    Texture2DConfig config;
    config.internalFormat = GL_RGBA8;
    config.sRGB = pixels.options.sRGB;
    config.generateMipmaps = pixels.options.generateMipmaps;
    config.sampler = pixels.options.sampler;

    auto texture = std::make_shared<Texture2D>(pixels.width, pixels.height, config);
    texture->uploadPixels(pixels.data.data(), GL_RGBA, GL_UNSIGNED_BYTE);

    return texture;
}

} // namespace render
} // namespace BulletRender
