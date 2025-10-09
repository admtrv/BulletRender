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
#include "render/RenderPass.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <algorithm>
#include <string>
#include <memory>
#include <iostream>

namespace luchrender {
namespace render {

// fwd
class Model;
class Shader;
class IRenderPass;

class Renderer {
public:
    static void init();
    static void clear(float r, float g, float b, float a);
    static void render(const scene::Scene& scene);
    static void resizeViewport(int width, int height);

    static void registerPrePass(std::shared_ptr<IRenderPass> pass);
    static void registerPostPass(std::shared_ptr<IRenderPass> pass);

private:
    static void renderBasePass(const scene::Scene& scene, const glm::mat4& view, const glm::mat4& proj);

    static std::vector<std::shared_ptr<IRenderPass>> s_pre;
    static std::vector<std::shared_ptr<IRenderPass>> s_post;
};


} // namespace render
} // namespace luchrender