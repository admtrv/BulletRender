/*
 * Loop.cpp
 */

#include "Loop.h"

namespace luchrender {
namespace app {

Loop::Loop(scene::Scene& scene) : m_scene(scene) {}

void Loop::setClearColor(const glm::vec4& color)
{
    m_clear = color;
}

void Loop::run(const std::function<void(float)>& update)
{
    utils::FrameTimer timer;

    while (!Window::shouldClose())
    {
        float dt = timer.tick();

        Window::pollEvents();

        if (update)
        {
            update(dt);
        }

        int fbw;
        int fbh;
        Window::getSize(fbw, fbh);
        render::Renderer::resizeViewport(fbw, fbh);
        m_scene.setAspect(fbh > 0 ? float(fbw) / float(fbh) : 1.0f);

        render::Renderer::clear(m_clear.r, m_clear.g, m_clear.b, m_clear.a);
        render::Renderer::render(m_scene);
        Window::swapBuffers();
    }
}

} // namespace app
} // namespace luchrender
