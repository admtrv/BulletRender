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
    Transform() : m_position(0.0f), m_scale(1.0f), m_matrix(1.0f) {}

    // rebuild matrix from components
    void rebuildMatrix()
    {
        glm::mat4 I(1.0f);
        glm::mat4 T = glm::translate(I, m_position);
        glm::mat4 Rx = glm::rotate(I, m_rotation.x, glm::vec3(1,0,0));
        glm::mat4 Ry = glm::rotate(I, m_rotation.y, glm::vec3(0,1,0));
        glm::mat4 Rz = glm::rotate(I, m_rotation.z, glm::vec3(0,0,1));
        glm::mat4 S  = glm::scale(I, m_scale);

        // M = T * Rz * Ry * Rx * S
        m_matrix = T * Rz * Ry * Rx * S;
    }

    // direct matrix operations
    const glm::mat4& getMatrix() const { return m_matrix; }
    void setMatrix(const glm::mat4& mat) { m_matrix = mat; }

    // position
    void setPosition(const glm::vec3& pos)
    {
        m_position = pos;
        rebuildMatrix();
    }

    glm::vec3 getPosition() const
    {
        return m_position;
    }

    // scale
    void setScale(const glm::vec3& scale)
    {
        m_scale = scale;
        rebuildMatrix();
    }

    glm::vec3 getScale() const
    {
        return m_scale;
    }

    // rotation (in radians)
    void setRotation(const glm::vec3& rotation)
    {
        m_rotation = rotation;
        rebuildMatrix();
    }

    glm::vec3 getRotation() const
    {
        return m_rotation;
    }

    // apply transformations

    // translate
    void translate(const glm::vec3& delta)
    {
        m_position += delta;
        rebuildMatrix();
    }

    // rotate
    void rotate(const glm::vec3& axis, float angleRad)
    {
        // convert axis-angle to euler angles and add
        glm::quat q = glm::angleAxis(angleRad, axis);
        glm::quat currentQ = glm::quat(glm::mat3(
            glm::rotate(glm::mat4(1.0f), m_rotation.x, glm::vec3(1,0,0)) *
            glm::rotate(glm::mat4(1.0f), m_rotation.y, glm::vec3(0,1,0)) *
            glm::rotate(glm::mat4(1.0f), m_rotation.z, glm::vec3(0,0,1))
        ));
        glm::quat newQ = q * currentQ;
        m_rotation = glm::eulerAngles(newQ);
        rebuildMatrix();
    }

    void rotateX(const float angleRad)
    {
        m_rotation.x += angleRad;
        rebuildMatrix();
    }

    void rotateY(const float angleRad)
    {
        m_rotation.y += angleRad;
        rebuildMatrix();
    }

    void rotateZ(const float angleRad)
    {
        m_rotation.z += angleRad;
        rebuildMatrix();
    }

    void scale(const glm::vec3& scl)
    {
        m_scale *= scl;
        rebuildMatrix();
    }

    void scale(const float factor)
    {
        m_scale *= factor;
        rebuildMatrix();
    }

    void setIdentity()
    {
        m_position = glm::vec3(0.0f);
        m_rotation = glm::vec3(0.0f);
        m_scale = glm::vec3(1.0f);
        m_matrix = glm::mat4(1.0f);
    }

    // build from components
    void setFromComponents(const glm::vec3& pos, const glm::vec3& anglesRad, const glm::vec3& scl)
    {
        m_position = pos;
        m_rotation = anglesRad;
        m_scale = scl;
        rebuildMatrix();
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

        // extract rotation as euler angles
        glm::quat quat = glm::quat_cast(R);
        m_rotation = glm::eulerAngles(quat);
        rebuildMatrix();
    }

    // get rotation axes from matrix (x, y, z axes of local coordinate system)
    void getAxes(glm::vec3& outX, glm::vec3& outY, glm::vec3& outZ) const
    {
        outX = glm::vec3(m_matrix[0]);
        outY = glm::vec3(m_matrix[1]);
        outZ = glm::vec3(m_matrix[2]);
    }

private:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::mat4 m_matrix;

    // Transform* m_parent{nullptr};
};

} // namespace scene
} // namespace BulletRender
