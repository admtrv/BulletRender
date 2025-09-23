/*
 * renderer.cpp
 */

#include "renderer.h"

#define VERTEX_SHADER_PATH "shaders/normal.vert.glsl"
#define FRAG_SHADER_PATH "shaders/normal.frag.glsl"

static std::unique_ptr<render::Shader> gShader;

namespace render {

void Renderer::init()
{
    glEnable(GL_DEPTH_TEST);

    gShader = std::make_unique<Shader>();
    if(!gShader->loadFromFiles(VERTEX_SHADER_PATH, FRAG_SHADER_PATH))
    {
        std::cerr << "failed to load shaders\n";
    }
}

void Renderer::clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(const scene::Scene& scene)
{
    if (!gShader)
    {
        return;
    }

    gShader->bind();

    const camera::Camera* cam  = scene.getCamera();
    const light::Light* light = scene.getLight();

    if (!cam || !light)
    {
        std::cerr << "renderer: no camera or light set in scene\n";
        return;
    }

    const auto& objects = scene.getObjects();
    float aspect = scene.getAspect();

    for (const auto& object : objects)
    {
        if (!object.model) continue;

        gShader->setMat4("uModel", object.transform);
        gShader->setMat4("uView", cam->view());
        gShader->setMat4("uProj", cam->proj(aspect));
        gShader->setVec3("uLightDir", light->getDirection());

        for (const auto& mesh : object.model->getMeshes())
            mesh.draw();
    }
}

void Renderer::resizeViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}


} // namespace render
