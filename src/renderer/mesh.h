/*
 * mesh.h
 */

#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

namespace renderer {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh {
public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices);
    ~Mesh();
    void draw() const;

private:
    unsigned m_vao=0; // Vertex Arrays Object (VAO)
    unsigned m_vbo=0; // Vertex Buffer Object (VBO)
    unsigned m_ebo=0; // Element Buffer Object (EBO)

    unsigned m_indexCount=0;
};

} // namespace renderer
