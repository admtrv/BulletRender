/*
 * RenderPass.cpp
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
};

} // namespace render
} // namespace BulletRender
