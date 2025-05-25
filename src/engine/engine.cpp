/*
 * engine.h
 */

#include "engine.h"

namespace engine {

void Engine::run(const EngineConfig &config)
{
    const auto& windowConfig = config.window;

    if (!window::Window::init(windowConfig.width, windowConfig.height, windowConfig.title.c_str(), windowConfig.resizable))
    {
        return;
    }

    renderer::Renderer::init();

    while (!window::Window::shouldClose())
    {
        window::Window::pollEvents();

        renderer::Renderer::beginFrame();
        renderer::Renderer::clear(0.0f, 0.0f, 0.0f, 1.0f);
        renderer::Renderer::endFrame();

        window::Window::swapBuffers();
    }

    window::Window::shutdown();
}

} // namespace engine
