/*
 * FontLoader.cpp
 */

#include "FontLoader.h"

#include <fstream>
#include <iostream>

namespace BulletRender {
namespace render {

FontLoader& FontLoader::instance()
{
    static FontLoader inst;
    return inst;
}

std::shared_ptr<Font> FontLoader::load(const std::string& path)
{
    auto it = m_cache.find(path);
    if (it != m_cache.end())
    {
        if (auto cached = it->second.lock())
        {
            return cached;
        }
        m_cache.erase(it);
    }

    auto font = loadFromDisk(path);
    if (font)
    {
        m_cache[path] = font;
    }
    return font;
}

void FontLoader::remove(const std::string& path)
{
    m_cache.erase(path);
}

void FontLoader::clear()
{
    m_cache.clear();
}

std::shared_ptr<Font> FontLoader::loadFromDisk(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "font load failed: " << path << '\n';
        return nullptr;
    }

    std::vector<unsigned char> data(size_t(file.tellg()));

    file.seekg(0);
    file.read(reinterpret_cast<char*>(data.data()), std::streamsize(data.size()));

    auto font = std::make_shared<Font>(std::move(data));

    if (!font->isLoaded())
    {
        std::cerr << "font load failed: " << path << " (not a ttf)\n";
        return nullptr;
    }

    return font;
}

} // namespace render
} // namespace BulletRender
