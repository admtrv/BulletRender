/*
 * FontLoader.h
 */

#pragma once

#include "Font.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace BulletRender {
namespace render {

// classic asset cache for Font, keyed by path
class FontLoader {
public:
    static FontLoader& instance();

    std::shared_ptr<Font> load(const std::string& path);

    void remove(const std::string& path);     // next load reads the file again
    void clear();

private:
    FontLoader() = default;
    ~FontLoader() = default;

    FontLoader(const FontLoader&) = delete;
    FontLoader& operator=(const FontLoader&) = delete;

    std::shared_ptr<Font> loadFromDisk(const std::string& path);

    std::unordered_map<std::string, std::weak_ptr<Font>> m_cache;
};

} // namespace render
} // namespace BulletRender
