/*
 * Loop.cpp
 */

#include "Loop.h"

namespace BulletRender {
namespace app {

Loop::Loop(scene::Scene& scene) : m_scene(scene)
{
    // initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // initialize backends
    ImGui_ImplGlfw_InitForOpenGL(Window::get(), true);
    ImGui_ImplOpenGL3_Init(config::GLSLVersion);
}

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

        // ImGui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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

        // render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        Window::swapBuffers();
    }

    // cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

} // namespace app
} // namespace BulletRender
