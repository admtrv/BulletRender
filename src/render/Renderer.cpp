/*
 * Renderer.cpp
 */

#include "Renderer.h"

namespace BulletRender {
namespace render {

std::vector<std::shared_ptr<IRenderPass>> Renderer::s_pre;
std::vector<std::shared_ptr<IRenderPass>> Renderer::s_post;

void Renderer::init()
{
    glEnable(GL_DEPTH_TEST);
}

void Renderer::clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::resizeViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Renderer::registerPrePass(std::shared_ptr<IRenderPass> pass)
{
    s_pre.push_back(std::move(pass));
}

void Renderer::registerPostPass(std::shared_ptr<IRenderPass> pass)
{
    s_post.push_back(std::move(pass));
}

void Renderer::render(const scene::Scene& scene)
{
    // PrePass
    for (auto& p : s_pre)
    {
        p->render(scene);
    }

    // BasePass
    renderBasePass(scene);

    // PostPass
    for (auto& p : s_post)
    {
        p->render(scene);
    }
}

void Renderer::renderBasePass(const scene::Scene& scene)
{
    const scene::Camera* cam = scene.getCamera();
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

} // namespace render
} // namespace BulletRender