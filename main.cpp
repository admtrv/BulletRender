/*
 * main.cpp
 */

#include "app/Window.h"
#include "app/Loop.h"
#include "interface/Editor.h"
#include "render/passes/WorldAxis.h"
#include "render/passes/Grid.h"
#include "render/passes/Lines.h"
#include "render/passes/SkyBox.h"
#include "render/Renderer.h"
#include "render/DebugDraw.h"
#include "render/Shader.h"
#include "scene/Scene.h"
#include "scene/Camera.h"
#include "scene/Light.h"
#include "utils/Input.h"

using namespace BulletRender;

int main()
{
    // window
    app::WindowConfig windowCfg{1600, 900, "BulletRender", true, true};
    if (!app::Window::init(windowCfg))
    {
        return -1;
    }

    // renderer
    render::Renderer::init();

    auto skybox = std::make_shared<render::SkyBox>(nullptr);
    skybox->setEnabled(false);
    render::Renderer::registerPrePass(skybox);

    // grid
    auto grid = std::make_shared<render::Grid>();
    render::Renderer::registerPrePass(grid);

    // world coordinates
    auto worldAxis = std::make_shared<render::WorldAxis>();
    render::Renderer::registerPrePass(worldAxis);

    // lines
    auto lines = std::make_shared<render::Lines>();
    lines->setDepthTest(false);
    render::Renderer::registerOverlayPass(lines);

    // debug gizmos
    render::DebugDraw debug(lines);

    // default shader for everything
    auto shader = std::make_shared<render::GraphicsShader>(
        "assets/shaders/normal.vert.glsl",
        "assets/shaders/normal.frag.glsl"
    );

    // scene
    scene::Scene scene;

    // cursor starts free so panel is usable
    scene.createCamera<scene::FlyCamera>(glm::vec3{0.0f, 2.0f, 8.0f}, -90.0f, 0.0f, 60.0f, 3.0f, 0.1f, 100.0f, 0.1f);

    // default scene
    scene::AmbientLight* ambient = scene.createLight<scene::AmbientLight>();
    ambient->setColor({0.4f, 0.45f, 0.55f});
    ambient->setIntensity(0.3f);

    scene::DirectionalLight* sun = scene.createLight<scene::DirectionalLight>();
    sun->setColor({1.0f, 0.95f, 0.85f});
    sun->setIntensity(0.8f);
    sun->setPosition(5.0f * sun->getDirection());

    // default cube
    auto cubeModel = std::make_shared<scene::Box>(1.0f, 1.0f, 1.0f);
    scene::SceneObject* cube = scene.addObject(cubeModel, "Cube");
    cube->getMaterial().setShader(shader);
    cube->getTransform().setLocalPosition({0.0f, 0.0f, 0.0f});

    // editor
    interface::Editor editor(scene, debug);
    editor.setSkyBox(skybox);
    editor.setDefaultShader(shader);
    editor.setShowDebug(false);

    // pace the frames when vsync is off
    app::Loop::setFrameRateLimit(120);

    // controls
    utils::Input& input = utils::Input::instance();
    input.bindKey(utils::InputKey::ESCAPE, [] {
        app::Window::setShouldClose(true);
    });
    input.bindKey(utils::InputKey::G, [&] {
        editor.toggleShowDebug();
    });

    // loop
    app::Loop loop(scene);
    loop.setBeforeFrame([&] { editor.beforeFrame(); });
    loop.run(
        [&](float dt) {
            input.update();

            editor.draw(dt);

            // panel owns input while it is being used
            scene::Camera* camera = scene.getActiveCamera();
            if (camera != nullptr && !editor.wantsInput())
            {
                camera->update(dt);
            }

            if (editor.getShowDebug())
            {
                debug.drawScene(scene);
            }
        }
    );

    app::Window::shutdown();
    return 0;
}