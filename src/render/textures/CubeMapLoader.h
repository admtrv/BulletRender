/*
 * CubeMapLoader.h
 */

#pragma once

#include "CubeMap.h"
#include "TextureLoader.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace BulletRender {
namespace render {

// six faces, before gl has texture for them
struct CubeMapPixels {
    int faceSize = 0;
    std::array<std::vector<unsigned char>, CUBE_FACE_COUNT> faces;
    CubeMapConfig config;
    std::string path;

    bool empty() const { return faceSize == 0; }
};

class CubeMapLoader {
public:
    // reading, one file per face or cross laid out 4 wide and 3 tall:
    //       [+Y]
    //   [-X][+Z][+X][-Z]
    //       [-Y]
    static CubeMapPixels readFaces(const std::array<std::string, CUBE_FACE_COUNT>& paths, const CubeMapConfig& cfg = {});
    static CubeMapPixels readCross(const std::string& path, const CubeMapConfig& cfg = {});

    // uploading
    static std::shared_ptr<CubeMap> upload(const CubeMapPixels& pixels);
    static std::shared_ptr<CubeMap> uploadFaces(const std::array<const TexturePixels*, CUBE_FACE_COUNT>& faces, const CubeMapConfig& cfg = {});
};

} // namespace render
} // namespace BulletRender
