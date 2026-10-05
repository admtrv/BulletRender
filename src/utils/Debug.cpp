/*
 * Debug.cpp
 */

#include "Debug.h"

namespace BulletRender {
namespace utils {

DebugGroup::DebugGroup(const std::string& name)
{
    if (glPushDebugGroup && !name.empty())
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name.c_str());
        m_pushed = true;
    }
}

DebugGroup::~DebugGroup()
{
    if (m_pushed)
    {
        glPopDebugGroup();
    }
}

// blank label reads worse than plain id
void setLabel(GLenum type, unsigned id, const std::string& name)
{
    if (glObjectLabel && !name.empty())
    {
        glObjectLabel(type, id, -1, name.c_str());
    }
}

} // namespace utils
} // namespace BulletRender
