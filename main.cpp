/*
 * main.cpp
 */

#include "app/Window.h"
#include "app/Loop.h"
#include "render/passes/WorldAxis.h"
#include "render/passes/Grid.h"
#include "render/Renderer.h"
#include "render/Shader.h"
#include "scene/Scene.h"
#include "scene/Model.h"
#include "scene/Camera.h"
#include "scene/Light.h"

using namespace luchrender;

int main()
{
    // window
    app::WindowConfig windowCfg{800, 600, "Demo", true};
    if (!app::Window::init(windowCfg))
    {
        return -1;
    }

    // renderer
    render::Renderer::init();

    // grid
    auto grid = std::make_shared<render::Grid>();
    render::Renderer::registerPrePass(grid);

    // world coordinated
    auto worldAxis = std::make_shared<render::WorldAxis>();
    render::Renderer::registerPrePass(worldAxis);

    // assets
    scene::Model model("assets/models/fox.obj");
    std::shared_ptr<render::Shader> shader = std::make_shared<render::Shader>(
        "assets/shaders/normal.vert.glsl",
        "assets/shaders/normal.frag.glsl"
    );

    // scene
    scene::Scene scene;

    scene::FlyCamera camera({0, 1, 5});
    scene.setCamera(&camera);

    scene::DirectionalLight light;
    scene.setLight(&light);

    scene::SceneObject* object = scene.addObject(&model);
    object->getMaterial().setShader(shader);
    object->getMaterial().setColor({1.0f, 0.5f, 0.0f});
    object->getTransform().setPosition({0, 0, 0});

    // loop
    app::Loop loop(scene);
    loop.run(
        [&](float dt) {
            camera.update(app::Window::get(), dt);
            object->getTransform().rotateZ(0.5f * dt);
        }
    );

    app::Window::shutdown();
    return 0;
}