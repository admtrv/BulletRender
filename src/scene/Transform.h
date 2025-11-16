/*
 * Transform.h
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>

namespace BulletRender {
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

    // get position from transformation matrix
    glm::vec3 getPosition() const
    {
        return glm::vec3(m_matrix[3]);
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

    void rotateFromDirection(const glm::vec3& fromDir, const glm::vec3& toDir)
    {
        glm::vec3 from = glm::normalize(fromDir);
        glm::vec3 to = glm::normalize(toDir);

        // check if directions are same
        if (glm::length2(from - to) < 1e-6f)
        {
            return; // already pointing same direction
        }

        // check if directions are opposite
        if (glm::length2(from + to) < 1e-6f)
        {
            glm::vec3 axis;
            if (glm::abs(from.x) < 0.9f)
            {
                axis = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            else
            {
                axis = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            axis = glm::normalize(glm::cross(from, axis));
            rotate(axis, glm::pi<float>());
            return;
        }

        // compute rotation from to
        glm::quat q = glm::rotation(from, to);
        glm::mat4 R = glm::toMat4(q);

        m_matrix[0] = glm::vec4(R[0].x, R[0].y, R[0].z, 0.0f);
        m_matrix[1] = glm::vec4(R[1].x, R[1].y, R[1].z, 0.0f);
        m_matrix[2] = glm::vec4(R[2].x, R[2].y, R[2].z, 0.0f);
    }

    // get rotation axes from matrix (x, y, z axes of local coordinate system)
    void getAxes(glm::vec3& outX, glm::vec3& outY, glm::vec3& outZ) const
    {
        outX = glm::vec3(m_matrix[0]);
        outY = glm::vec3(m_matrix[1]);
        outZ = glm::vec3(m_matrix[2]);
    }

private:
    glm::mat4 m_matrix;
    // Transform* m_parent{nullptr};
};

} // namespace scene
} // namespace BulletRender
