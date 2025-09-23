/*
 * camera.h
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <cmath>

struct GLFWwindow;

namespace luchrender {
namespace camera {

// basic camera
class Camera {
public:
    virtual ~Camera() = default;

    virtual glm::mat4 view() const = 0;
    virtual glm::mat4 proj(float aspect) const = 0;
    virtual void update(GLFWwindow* win, float dt) {}
};

// static camera (fixed view/proj)
class StaticCamera : public Camera {
public:
    StaticCamera(const glm::mat4& v, const glm::mat4& p) : m_view(v), m_proj(p) {}

    glm::mat4 view() const override { return m_view; }
    glm::mat4 proj(float) const override { return m_proj; }

private:
    glm::mat4 m_view;
    glm::mat4 m_proj;
};

// dynamic (flying) camera
class FlyCamera : public Camera {
public:
    explicit FlyCamera(glm::vec3 pos = {0,1,5}, float yaw = -90.f, float pitch = 0.f, float fovDeg = 30.f, float speed = 2.f, float zNear = 0.1f, float zFar = 100.f);

    glm::mat4 view() const override;
    glm::mat4 proj(float aspect) const override;
    void update(GLFWwindow* win, float dt) override;

private:
    glm::vec3 m_pos;
    float m_yaw;
    float m_pitch;
    float m_fovDeg;
    float m_speed;
    float m_zNear;
    float m_zFar;
};

} // namespace camera
} // namespace luchrender