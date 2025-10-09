/*
 * Grid.cpp
 */

#include "Grid.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <glad/glad.h>

namespace luchrender {
namespace render {

Grid::Grid(bool enabled) : m_enabled(enabled) {
    m_prog = std::make_shared<Shader>(VERT_PATH, FRAG_PATH);
    glGenVertexArrays(1, &m_Vao);
}

Grid::~Grid() {
    if (m_Vao)
    {
        glDeleteVertexArrays(1, &m_Vao);
    }
}

void Grid::render(const scene::Scene& scene, const glm::mat4& view, const glm::mat4& proj)
{
    if (!m_enabled || !m_prog)
    {
        return;
    }

    glm::mat4 invVP = glm::inverse(proj * view);
    glm::mat4 VP = proj * view;

    // depth
    GLboolean depthEnabled = glIsEnabled(GL_DEPTH_TEST);
    if (!depthEnabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    m_prog->bind();
    m_prog->setMat4("uInvViewProj", invVP);
    m_prog->setMat4("uViewProj", VP);

    glBindVertexArray(m_Vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    if (!depthEnabled)
    {
        glDisable(GL_DEPTH_TEST);
    }
}

} // namespace render
} // namespace luchrender
