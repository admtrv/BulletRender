/*
* Camera.h
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <cmath>

struct GLFWwindow;

namespace luchrender {
namespace scene {

#define YAW_LIMIT   89.0f
#define WORLD_UP    {0.0f, 1.0f, 0.0f}

// basic camera
class Camera {
public:
    virtual ~Camera() = default;

    virtual glm::mat4 view() const = 0;
    virtual glm::mat4 proj(float aspect) const = 0;

    virtual float near() const = 0;
    virtual float far() const = 0;

    virtual void update(GLFWwindow* win, float dt) {}
};

// static camera (fixed view/proj)
class StaticCamera : public Camera {
public:
    StaticCamera(const glm::mat4& v, const glm::mat4& p, float zNear = 0.1f, float zFar = 100.0f)
        : m_view(v), m_proj(p), m_zNear(zNear), m_zFar(zFar) {}

    glm::mat4 view() const override { return m_view; }
    glm::mat4 proj(float) const override { return m_proj; }

    float near() const override { return m_zNear; }
    float far()  const override { return m_zFar; }

private:
    glm::mat4 m_view;
    glm::mat4 m_proj;

    float m_zNear;
    float m_zFar;
};

// fly camera (Move: WASD, Camera: Mouse, Boost: Shift, Toggle Cursor: F1)
class FlyCamera : public Camera {
public:
    explicit FlyCamera(glm::vec3 pos = {0,1,5},
                       float yaw = -90.f,
                       float pitch = 0.f,
                       float fovDeg = 60.f,
                       float speed = 3.f,
                       float zNear = 0.1f,
                       float zFar = 100.f,
                       float mouseSensitivity = 0.1f,
                       bool lockCursor = true);

    glm::mat4 view() const override;
    glm::mat4 proj(float aspect) const override;

    float near() const override { return m_zNear; }
    float far()  const override { return m_zFar; }

    void update(GLFWwindow* win, float dt) override;

private:
    enum class CursorMode {Locked, Free};

    glm::vec3 forwardDir() const;
    void applyCursorMode(GLFWwindow* win);
    void toggleCursorMode(GLFWwindow* win);

    glm::vec3 m_pos;
    float m_yaw;
    float m_pitch;
    float m_fovDeg;
    float m_speed;
    float m_zNear;
    float m_zFar;

    float m_sensitivity;
    CursorMode m_mode;
    bool m_mouseInit;
    double m_lastX;
    double m_lastY;

    bool m_prevTogglePressed = false;
};

} // namespace scene
} // namespace luchrender
