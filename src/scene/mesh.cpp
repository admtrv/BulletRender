/*
 * mesh.cpp
 */

#include "mesh.h"

namespace luchrender {
namespace scene {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices) {
    // index count
    m_indexCount = (unsigned)indices.size();

    // create buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    // bind vao
    glBindVertexArray(m_vao);

    // bind and set vbo
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // bind and set ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

    // attribute 0 - position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*) 0);

    // attribute 1 - normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*) offsetof(Vertex,normal));

    // end vao tuning
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    if(m_ebo)
    {
        glDeleteBuffers(1, &m_ebo);
    }

    if(m_vbo)
    {
        glDeleteBuffers(1, &m_vbo);
    }

    if(m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
}

void Mesh::draw() const
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

} // namespace scene
} // namespace luchrender