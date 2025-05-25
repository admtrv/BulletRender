/*
 * renderer.cpp
 */

#include "renderer.h"

#include <glad/glad.h>

namespace renderer {

void Renderer::init()
{

}

void Renderer::clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::beginFrame()
{

}

void Renderer::endFrame()
{

}

} // namespace renderer
