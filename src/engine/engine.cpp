/*
 * engine.h
 */

#include "engine.h"
#include "../renderer/model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

void Engine::run(const EngineConfig &config)
{
    const auto& wc = config.window;
    if (!window::Window::init(wc.width, wc.height, wc.title.c_str(), wc.resizable))
        return;

    renderer::Renderer::init();

    // загружаем модель
    renderer::Model model;
    if(!model.loadObj("assets/models/fox.obj"))
        std::cerr << "Failed to load assets/models/fox.obj\n";

    // простая камера
    glm::vec3 camPos = {0.0f, 1.5f, 3.5f};
    glm::vec3 camTarget = {0.0f, 0.7f, 0.0f};

    while (!window::Window::shouldClose())
    {
        window::Window::pollEvents();

        int fbw, fbh;
        glfwGetFramebufferSize(window::Window::get(), &fbw, &fbh);
        float aspect = fbh>0 ? float(fbw)/float(fbh) : 1.0f;

        glm::mat4 view = glm::lookAt(camPos, camTarget, {0,1,0});
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
        renderer::Scene scene{view, proj, glm::normalize(glm::vec3(-0.4f,1.0f,0.2f))};

        // модель
        glm::mat4 modelM(1.0f);
        modelM = glm::scale(modelM, glm::vec3(1.0f));

        renderer::Renderer::beginFrame();
        renderer::Renderer::clear(0.05f,0.05f,0.08f,1.0f);

        renderer::Renderer::drawModel(model, scene, modelM);

        renderer::Renderer::endFrame();
        window::Window::swapBuffers();
    }

    window::Window::shutdown();
}

} // namespace engine
