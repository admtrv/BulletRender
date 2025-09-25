/*
 * transform.h
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace luchrender {
namespace scene {

// for now just wcs
class Transform {
public:
    Transform() : m_matrix(1.0f) {}

    // direct matrix operations
    const glm::mat4& getMatrix() const { return m_matrix; }
    void setMatrix(const glm::mat4& mat) { m_matrix = mat; }

    // apply transformations to current matrix

    // translate
    void translate(const glm::vec3& delta)
    {
        m_matrix = glm::translate(m_matrix, delta); // m = m * T
    }

    // rotate
    void rotate(const glm::vec3& axis, float angleRad)
    {
        m_matrix = glm::rotate(m_matrix, angleRad, axis); // m = m * R
    }

    void rotateX(const float angleRad)
    {
        m_matrix = glm::rotate(m_matrix, angleRad, glm::vec3(1, 0, 0));
    }

    void rotateY(const float angleRad)
    {
        m_matrix = glm::rotate(m_matrix, angleRad, glm::vec3(0, 1, 0));
    }

    void rotateZ(const float angleRad)
    {
        m_matrix = glm::rotate(m_matrix, angleRad, glm::vec3(0, 0, 1));
    }

    // scale
    void scale(const glm::vec3& scale)
    {
        m_matrix = glm::scale(m_matrix, scale); // m = m * S
    }

    void scale(const float factor)
    {
        m_matrix = glm::scale(m_matrix, glm::vec3(factor));
    }

    // reset to specific absolute transform
    void setPosition(const glm::vec3& pos)
    {
        m_matrix[3] = glm::vec4(pos, 1.0f);
    }

    void setIdentity()
    {
        m_matrix = glm::mat4(1.0f);
    }

    // build from components
    void setFromComponents(const glm::vec3& pos, const glm::vec3& anglesRad, const glm::vec3& scl)
    {
        glm::mat4 I(1.0f);
        glm::mat4 T = glm::translate(I, pos);
        glm::mat4 Rx = glm::rotate(I, anglesRad.x, glm::vec3(1,0,0));
        glm::mat4 Ry = glm::rotate(I, anglesRad.y, glm::vec3(0,1,0));
        glm::mat4 Rz = glm::rotate(I, anglesRad.z, glm::vec3(0,0,1));
        glm::mat4 S  = glm::scale(I, scl);

        // M = T * Rz * Ry * Rx * S or M = T * R * S
        m_matrix = T * Rz * Ry * Rx * S;
    }

    // apply another transform
    void applyTransform(const glm::mat4& rhs)
    {
        m_matrix = m_matrix * rhs;
    }

private:
    glm::mat4 m_matrix;
    // Transform* m_parent{nullptr};
};

} // namespace scene
} // namespace luchrender
