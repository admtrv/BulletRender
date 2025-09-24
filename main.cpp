/*
 * main.cpp
 */

#include "app/window.h"
#include "app/loop.h"
#include "render/renderer.h"
#include "render/shader.h"
#include "scene/scene.h"
#include "scene/model.h"
#include "camera/camera.h"
#include "light/light.h"

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

    // camera
    camera::FlyCamera camera({0,1.5f,5.0f});

    // light
    light::DirectionalLight light;

    // model
    scene::Model model("assets/models/fox.obj");

    // shader
    auto shader = std::make_shared<render::Shader>("assets/shaders/normal.vert.glsl", "assets/shaders/normal.frag.glsl");

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