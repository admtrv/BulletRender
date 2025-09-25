/*
 * renderer.h
 */

#pragma once

#include "shader.h"
#include "scene/model.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/light.h"

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