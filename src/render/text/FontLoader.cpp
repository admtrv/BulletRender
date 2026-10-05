/*
 * FontLoader.cpp
 */

#include "FontLoader.h"

#include <fstream>
#include <iostream>

namespace BulletRender {
namespace render {

std::vector<unsigned char> FontLoader::read(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file)
    {
        std::cerr << "font read failed: " << path << '\n';
        return {};
    }

    std::vector<unsigned char> data(size_t(file.tellg()));

    file.seekg(0);
    file.read(reinterpret_cast<char*>(data.data()), std::streamsize(data.size()));

    return data;
}

std::shared_ptr<Font> FontLoader::upload(std::vector<unsigned char> data)
{
    if (data.empty())
    {
        return nullptr;
    }

    auto font = std::make_shared<Font>(std::move(data));

    if (!font->isLoaded())
    {
        std::cerr << "font load failed, not a ttf\n";
        return nullptr;
    }

    return font;
}

} // namespace render
} // namespace BulletRender
