/*
 * Canvas.cpp
 */

#include "Canvas.h"

#include "render/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace BulletRender {
namespace render {

constexpr unsigned CANVAS_TEXTURE_UNIT = 0;
constexpr size_t VERTICES_PER_QUAD = 6;

Canvas::Canvas()
{
    m_program = std::make_shared<GraphicsShader>(CANVAS_VERT_PATH, CANVAS_FRAG_PATH);

    // single white pixel, so plain color is image of nothing
    m_blank = std::make_unique<Texture2D>(1, 1);

    const unsigned char white[4] = {255, 255, 255, 255};
    m_blank->uploadPixels(white, GL_RGBA, GL_UNSIGNED_BYTE);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CanvasVertex), (void*)offsetof(CanvasVertex, pos));

    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CanvasVertex), (void*)offsetof(CanvasVertex, uv));

    glEnableVertexAttribArray(2); // color
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CanvasVertex), (void*)offsetof(CanvasVertex, color));

    glEnableVertexAttribArray(3); // coverage
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(CanvasVertex), (void*)offsetof(CanvasVertex, coverage));

    glBindVertexArray(0);
}

Canvas::~Canvas()
{
    if (m_vbo)
    {
        glDeleteBuffers(1, &m_vbo);
    }
    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
}

void Canvas::addRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    addQuad(m_blank.get(), position, size, {0.0f, 0.0f}, {1.0f, 1.0f}, color);
}

void Canvas::addImage(const Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& tint)
{
    addQuad(texture ? texture : m_blank.get(), position, size, {0.0f, 0.0f}, {1.0f, 1.0f}, tint);
}

// reads one utf8 codepoint, moving cursor past bytes it took
static uint32_t nextCodepoint(const std::string& text, size_t& cursor)
{
    const auto byte = [&text](size_t at) { return uint32_t(static_cast<unsigned char>(text[at])); };

    const uint32_t lead = byte(cursor++);

    // leading byte says how many follow it
    int trailing = 0;
    uint32_t codepoint = lead;

    if ((lead & 0xE0u) == 0xC0u)      { trailing = 1; codepoint = lead & 0x1Fu; }
    else if ((lead & 0xF0u) == 0xE0u) { trailing = 2; codepoint = lead & 0x0Fu; }
    else if ((lead & 0xF8u) == 0xF0u) { trailing = 3; codepoint = lead & 0x07u; }

    while (trailing-- > 0 && cursor < text.size() && (byte(cursor) & 0xC0u) == 0x80u)
    {
        codepoint = (codepoint << 6) | (byte(cursor++) & 0x3Fu);
    }

    return codepoint;
}

// walks string once, handing every glyph its box, so drawing and measuring agree
template <class Step>
static glm::vec2 walkText(Font& font, const std::string& text, float size, Step step)
{
    const FontMetrics metrics = font.getMetrics(size);

    float pen = 0.0f;
    uint32_t previous = 0;

    for (size_t cursor = 0; cursor < text.size(); )
    {
        const uint32_t codepoint = nextCodepoint(text, cursor);

        if (previous)
        {
            pen += font.getKerning(previous, codepoint, size);
        }

        if (const Glyph* glyph = font.getGlyph(codepoint, size))
        {
            step(*glyph, pen);
            pen += glyph->advance;
        }

        previous = codepoint;
    }

    return {pen, metrics.lineHeight};
}

void Canvas::addText(Font& font, const std::string& text, const glm::vec2& position, float size, const glm::vec4& color)
{
    const float baseline = position.y + font.getMetrics(size).ascent;

    walkText(font, text, size, [&](const Glyph& glyph, float pen) {
        if (glyph.size.x <= 0.0f || glyph.size.y <= 0.0f)
        {
            return;
        }

        const glm::vec2 corner{position.x + pen + glyph.bearing.x, baseline + glyph.bearing.y};

        addQuad(font.getAtlas(), corner, glyph.size, glyph.uvMin, glyph.uvMax, color, true);
    });
}

glm::vec2 Canvas::measureText(Font& font, const std::string& text, float size)
{
    return walkText(font, text, size, [](const Glyph&, float) {});
}

void Canvas::addQuad(const Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec2& uvMin, const glm::vec2& uvMax, const glm::vec4& color, bool coverage)
{
    // nothing to see, and empty quad would still cost draw
    if (size.x <= 0.0f || size.y <= 0.0f)
    {
        return;
    }

    // quads on one texture draw together, new one opens batch
    if (m_batches.empty() || m_batches.back().texture != texture)
    {
        m_batches.push_back({texture, 0});
    }

    const glm::vec2 min = position;
    const glm::vec2 max = position + size;

    const float flag = coverage ? 1.0f : 0.0f;

    const CanvasVertex topLeft     = {{min.x, min.y}, {uvMin.x, uvMin.y}, color, flag};
    const CanvasVertex topRight    = {{max.x, min.y}, {uvMax.x, uvMin.y}, color, flag};
    const CanvasVertex bottomRight = {{max.x, max.y}, {uvMax.x, uvMax.y}, color, flag};
    const CanvasVertex bottomLeft  = {{min.x, max.y}, {uvMin.x, uvMax.y}, color, flag};

    m_vertices.insert(m_vertices.end(), {topLeft, topRight, bottomRight, topLeft, bottomRight, bottomLeft});
    m_batches.back().count += VERTICES_PER_QUAD;
}

void Canvas::render(const scene::Scene&)
{
    if (m_batches.empty())
    {
        return;
    }

    const glm::ivec2 viewport = Renderer::getViewport();

    // pixels as they are read, y running down from top left corner
    const glm::mat4 proj = glm::ortho(0.0f, float(viewport.x), float(viewport.y), 0.0f);

    // flat layer sits over frame, ignoring depth scene left behind
    const GLboolean depthEnabled = glIsEnabled(GL_DEPTH_TEST);
    const GLboolean blendEnabled = glIsEnabled(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_program->bind();
    m_program->setMat4("uProj", proj);
    m_program->setInt("uTexture", CANVAS_TEXTURE_UNIT);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(m_vertices.size() * sizeof(CanvasVertex)), m_vertices.data(), GL_DYNAMIC_DRAW);

    GLint first = 0;
    for (const Batch& batch : m_batches)
    {
        batch.texture->bind(CANVAS_TEXTURE_UNIT);

        glDrawArrays(GL_TRIANGLES, first, GLint(batch.count));
        first += GLint(batch.count);
    }

    glBindVertexArray(0);

    // restore whatever caller had
    if (depthEnabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    if (!blendEnabled)
    {
        glDisable(GL_BLEND);
    }
}

void Canvas::endFrame()
{
    m_vertices.clear();
    m_batches.clear();
}

} // namespace render
} // namespace BulletRender
