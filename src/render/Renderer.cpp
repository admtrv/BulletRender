/*
 * Renderer.cpp
 */

#include "Renderer.h"

namespace luchrender {
namespace render {

void Renderer::init()
{
    glEnable(GL_DEPTH_TEST);
}

void Renderer::clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(const scene::Scene& scene)
{
    const scene::Camera* cam= scene.getCamera();
    const scene::Light* light = scene.getLight();

    if (!cam || !light)
    {
        std::cerr << "renderer: no camera or light set in scene\n";
        return;
    }

    for (const auto& object : scene.getObjects())
    {
        if (!object)
        {
            continue;
        }

        const scene::Model* model = object->getModel();
        if (!model)
        {
            continue;
        }

        auto shader = object->getMaterial().getShader();
        if (!shader)
        {
            continue;
        }

        shader->bind();

        // common uniforms
        shader->setMat4("uView", cam->view());
        shader->setMat4("uProj", cam->proj(scene.getAspect()));
        shader->setVec3("uLightDir", light->getDirection());

        // model uniforms
        shader->setMat4("uModel", object->getTransform().getMatrix());
        shader->setVec3("uColor", object->getMaterial().getColor());

        for (const auto& mesh : model->getMeshes())
        {
            mesh.draw();
        }
    }
}

void Renderer::resizeViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

} // namespace render
} // namespace luchrender