/*
 * Debug.h
 */

#pragma once

#include <glad/glad.h>

#include <string>

namespace BulletRender {
namespace utils {

// nests gl calls under one name in frame capture
class DebugGroup {
public:
    explicit DebugGroup(const std::string& name);
    ~DebugGroup();

    DebugGroup(const DebugGroup&) = delete;
    DebugGroup& operator=(const DebugGroup&) = delete;

private:
    bool m_pushed = false;
};

void setLabel(GLenum type, unsigned id, const std::string& name);

} // namespace utils
} // namespace BulletRender
