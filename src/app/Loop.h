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

    void run(const std::function<void(float)>& update);

private:
    scene::Scene& m_scene;
};

} // namespace app
} // namespace BulletRender
