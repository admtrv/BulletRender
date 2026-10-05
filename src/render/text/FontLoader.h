/*
 * FontLoader.h
 */

#pragma once

#include "Font.h"

#include <memory>
#include <string>
#include <vector>

namespace BulletRender {
namespace render {

// atlas is rasterized on draw, so only file is read ahead
class FontLoader {
public:
    static std::vector<unsigned char> read(const std::string& path);
    static std::shared_ptr<Font> upload(std::vector<unsigned char> data);
};

} // namespace render
} // namespace BulletRender
