/*
 * renderer.h
 */

#pragma once

#include "shader.h"
#include "model.h"
#include "mesh.h"
#include "scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <memory>
#include <iostream>

namespace renderer {

// fwd
class Model;
class Shader;

class Renderer {
public:
    static void init();
    static void clear(float r, float g, float b, float a);
    static void render(const Scene& scene);

    static void resizeViewport(int width, int height);

};

} // namespace renderer
