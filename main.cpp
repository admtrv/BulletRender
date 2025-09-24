/*
 * main.cpp
 */

#include "window/window.h"
#include "render/renderer.h"
#include "scene/scene.h"
#include "scene/model.h"
#include "camera/camera.h"
#include "light/light.h"
#include "utils/time.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace luchrender;

int main()
{
    // window
    window::WindowConfig windowCfg{800, 600, "Demo", true};
    if (!window::Window::init(windowCfg))
    {
        return -1;
    }

    // renderer
    render::Renderer::init();

    // camera
    camera::FlyCamera camera({0,1.5f,5.0f});

    // light
    light::DirectionalLight light;

    // model
    scene::Model model("assets/models/fox.obj");

    // shader
    auto shader = std::make_shared<render::Shader>("shaders/normal.vert.glsl", "shaders/normal.frag.glsl");

    // scene
    scene::Scene scene;
    scene.setCamera(&camera);
    scene.setLight(&light);

    scene::SceneObject* object = scene.addObject(&model);

    // tune object
    object->getMaterial().setShader(shader);
    object->getMaterial().setColor({1.0f, 0.5f, 0.0f});
    object->getTransform().setPosition({0, 0, 0});

    // time
    utils::FrameTimer timer;

    // loop
    while (!window::Window::shouldClose())
    {
        float dt = timer.tick();

        window::Window::pollEvents();

        // update camera
        camera.update(window::Window::get(), dt);

        // rotate object continuously
        object->getTransform().rotate({0, 0, 0.5}, dt);

        // framebuffer size + aspect
        int fbw;
        int fbh;
        window::Window::getSize(fbw, fbh);
        render::Renderer::resizeViewport(fbw, fbh);
        scene.setAspect(fbh > 0 ? float(fbw) / float(fbh) : 1.0f);

        // render
        render::Renderer::clear(0.05f, 0.05f, 0.08f, 1.0f);
        render::Renderer::render(scene);
        window::Window::swapBuffers();
    }

    window::Window::shutdown();
    return 0;
}