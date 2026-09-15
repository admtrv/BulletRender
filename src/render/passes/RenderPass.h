/*
 * RenderPass.h
 */

#pragma once

#include "scene/Scene.h"

#include <glm/glm.hpp>

namespace BulletRender {
namespace render {

class IRenderPass {
public:
    virtual ~IRenderPass() = default;
    virtual void render(const scene::Scene& scene) = 0;

    // disabled pass draws nothing, renderer can skip this work
    virtual bool isActive() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

protected:
    bool m_enabled = true;
};

} // namespace render
} // namespace BulletRender
