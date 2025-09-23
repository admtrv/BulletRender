/*
 * renderer.h
 */

#pragma once

#include "shader.h"
#include "model.h"
#include "mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <memory>
#include <iostream>

namespace renderer {

// fwd
class Model;
class Shader;

struct Scene {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 lightDir = glm::normalize(glm::vec3(0.3f,1.0f,0.2f));
};

class Renderer {
public:
    static void init();
    static void clear(float r, float g, float b, float a);
    static void beginFrame();
    static void endFrame();

    static void drawModel(const Model& model, const Scene& scene, const glm::mat4& modelMatrix);
};

} // namespace renderer
