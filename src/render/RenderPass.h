/*
 * RenderPass.cpp
 */

#pragma once

#include "scene/Scene.h"

#include <glm/glm.hpp>

namespace luchrender {
namespace render {

class IRenderPass {
public:
    virtual ~IRenderPass() = default;
    virtual void render(const scene::Scene& scene, const glm::mat4& view, const glm::mat4& proj) = 0;
};

} // namespace render
} // namespace luchrender
