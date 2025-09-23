/*
 * renderer.cpp
 */

#include "renderer.h"

#define VERTEX_SHADER_PATH "shaders/normal.vert.glsl"
#define FRAG_SHADER_PATH "shaders/normal.frag.glsl"

static std::unique_ptr<renderer::Shader> gShader;

namespace renderer {

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

void Renderer::beginFrame() {}
void Renderer::endFrame() {}

void Renderer::drawModel(const Model& model, const Scene& scene, const glm::mat4& modelMatrix)
{
    gShader->bind();

    gShader->setMat4("uModel", modelMatrix);
    gShader->setMat4("uView",  scene.view);
    gShader->setMat4("uProj",  scene.proj);
    gShader->setVec3("uLightDir", scene.lightDir);

    for (const auto& mesh : model.getMeshes())
    {
        mesh.draw();
    }
}

} // namespace renderer
