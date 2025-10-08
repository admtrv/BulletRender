/*
 * Renderer.h
 */

#pragma once

#include "Shader.h"
#include "scene/Model.h"
#include "scene/Mesh.h"
#include "scene/Scene.h"
#include "scene/Camera.h"
#include "scene/Light.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <memory>
#include <iostream>

namespace luchrender {
namespace render {

// fwd
class Model;
class Shader;

class Renderer {
public:
    static void init();
    static void clear(float r, float g, float b, float a);
    static void render(const scene::Scene& scene);
    static void resizeViewport(int width, int height);

};

} // namespace render
} // namespace luchrender