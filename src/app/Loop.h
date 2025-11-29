/*
 * Loop.h
 */

#pragma once

#include "Config.h"
#include "Window.h"
#include "render/Renderer.h"
#include "scene/Scene.h"
#include "utils/Time.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <functional>
#include <glm/glm.hpp>

namespace BulletRender {
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
} // namespace BulletRender
