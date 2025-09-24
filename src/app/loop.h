/*
 * loop.h
 */

#pragma once

#include "window.h"
#include "render/renderer.h"
#include "scene/scene.h"
#include "utils/time.h"

#include <functional>
#include <glm/glm.hpp>

namespace luchrender {
namespace app {

class Loop {
public:
    explicit Loop(scene::Scene& scene);

    void setClearColor(const glm::vec4& color);

    void run(const std::function<void(float)>& update);

private:
    scene::Scene& m_scene;
    glm::vec4 m_clear{0.05f, 0.05f, 0.08f, 1.0f};
};

} // namespace app
} // namespace luchrender
