/*
 * Canvas.h
 */

#pragma once

#include "RenderPass.h"
#include "render/Shader.h"
#include "render/textures/Texture2D.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <memory>
#include <vector>

namespace BulletRender {
namespace render {

#define CANVAS_VERT_PATH "assets/shaders/canvas.vert.glsl"
#define CANVAS_FRAG_PATH "assets/shaders/canvas.frag.glsl"

struct CanvasVertex {
    glm::vec2 pos;
    glm::vec2 uv;
    glm::vec4 color;
};

// flat layer over frame, drawn in pixels from top left corner
//
//   (0,0)              (w,0)
//     +------------------+
//     |                  |
//     +------------------+
//   (0,h)              (w,h)

class Canvas final : public IRenderPass {
public:
    Canvas();
    ~Canvas();

    // api for frame
    void addRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void addImage(const Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& tint = glm::vec4(1.0f));

    void render(const scene::Scene& scene) override;

    // drops what was added, whether views drew it or not
    void endFrame();

private:
    // quads sharing texture go out in one call
    struct Batch {
        const Texture* texture = nullptr;
        size_t count = 0;               // vertices, six per quad
    };

    void addQuad(const Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec2& uvMin, const glm::vec2& uvMax, const glm::vec4& color);

    std::shared_ptr<GraphicsShader> m_program;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;

    // stands in where quad carries color alone
    std::unique_ptr<Texture2D> m_blank;

    std::vector<CanvasVertex> m_vertices;
    std::vector<Batch> m_batches;
};

} // namespace render
} // namespace BulletRender
