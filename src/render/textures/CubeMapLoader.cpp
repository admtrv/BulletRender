/*
 * CubeMapLoader.cpp
 */

#include "CubeMapLoader.h"

#include <stb_image.h>

#include <cstring>
#include <iostream>

namespace BulletRender {
namespace render {

constexpr int CHANNELS = 4;

// where each face sits in cross:
//          [+Y]
//   [-X][+Z][+X][-Z]
//          [-Y]
struct Cell {
    int col;
    int row;
};

static constexpr Cell CROSS_CELLS[CUBE_FACE_COUNT] = {
    {2, 1},     // +X
    {0, 1},     // -X
    {1, 0},     // +Y
    {1, 2},     // -Y
    {1, 1},     // +Z
    {3, 1}      // -Z
};

CubeMapPixels CubeMapLoader::readFaces(const std::array<std::string, CUBE_FACE_COUNT>& paths, const CubeMapConfig& cfg)
{
    CubeMapPixels pixels;
    pixels.config = cfg;

    // readers run side by side, so flip must not reach past this one
    stbi_set_flip_vertically_on_load_thread(cfg.flipVertically ? 1 : 0);

    for (int face = 0; face < CUBE_FACE_COUNT; face++)
    {
        int width = 0, height = 0, channels = 0;
        stbi_uc* read = stbi_load(paths[face].c_str(), &width, &height, &channels, CHANNELS);

        if (!read)
        {
            std::cerr << "cubemap face read failed: " << paths[face] << " (" << stbi_failure_reason() << ")\n";
            return {};
        }

        if (width != height || (pixels.faceSize != 0 && width != pixels.faceSize))
        {
            std::cerr << "cubemap face expects square of one size, got " << width << "x" << height << ": " << paths[face] << "\n";
            stbi_image_free(read);
            return {};
        }

        pixels.faceSize = width;
        pixels.faces[face].assign(read, read + size_t(width) * height * CHANNELS);

        stbi_image_free(read);
    }

    return pixels;
}

CubeMapPixels CubeMapLoader::readCross(const std::string& path, const CubeMapConfig& cfg)
{
    CubeMapPixels pixels;
    pixels.config = cfg;

    stbi_set_flip_vertically_on_load_thread(cfg.flipVertically ? 1 : 0);

    int width = 0, height = 0, channels = 0;
    stbi_uc* read = stbi_load(path.c_str(), &width, &height, &channels, CHANNELS);

    if (!read)
    {
        std::cerr << "cubemap cross read failed: " << path << " (" << stbi_failure_reason() << ")\n";
        return {};
    }

    const int face = width / 4;

    if (face == 0 || width != face * 4 || height != face * 3)
    {
        std::cerr << "cubemap cross expects 4x3 layout, got " << width << "x" << height << "\n";
        stbi_image_free(read);
        return {};
    }

    pixels.faceSize = face;

    for (int index = 0; index < CUBE_FACE_COUNT; index++)
    {
        const Cell& cell = CROSS_CELLS[index];
        std::vector<unsigned char>& target = pixels.faces[index];

        target.resize(size_t(face) * face * CHANNELS);

        for (int row = 0; row < face; row++)
        {
            const stbi_uc* source = read + (size_t(cell.row * face + row) * width + cell.col * face) * CHANNELS;
            std::memcpy(target.data() + size_t(row) * face * CHANNELS, source, size_t(face) * CHANNELS);
        }
    }

    stbi_image_free(read);

    return pixels;
}

std::shared_ptr<CubeMap> CubeMapLoader::uploadFaces(const std::array<const TexturePixels*, CUBE_FACE_COUNT>& faces, const CubeMapConfig& cfg)
{
    const int size = faces[0] ? faces[0]->width : 0;

    for (const TexturePixels* face : faces)
    {
        if (!face || face->empty() || face->width != size || face->height != size)
        {
            std::cerr << "cubemap faces expect square of one size\n";
            return nullptr;
        }
    }

    auto cubemap = std::make_shared<CubeMap>(size, cfg);

    for (int face = 0; face < CUBE_FACE_COUNT; face++)
    {
        cubemap->uploadFace(face, faces[face]->data.data());
    }

    return cubemap;
}

std::shared_ptr<CubeMap> CubeMapLoader::upload(const CubeMapPixels& pixels)
{
    if (pixels.empty())
    {
        return nullptr;
    }

    auto cubemap = std::make_shared<CubeMap>(pixels.faceSize, pixels.config);

    for (int face = 0; face < CUBE_FACE_COUNT; face++)
    {
        cubemap->uploadFace(face, pixels.faces[face].data());
    }

    return cubemap;
}

} // namespace render
} // namespace BulletRender
