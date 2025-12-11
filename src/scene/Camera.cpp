/*
 * Camera.cpp
 */

#include "Camera.h"

#include <GLFW/glfw3.h>

namespace BulletRender {
namespace scene {

// StaticCamera

glm::mat4 StaticCamera::view() const
{
    return glm::lookAt(m_pos, m_target, m_up);
}

glm::mat4 StaticCamera::proj(float aspect) const
{
    return glm::perspective(glm::radians(m_fovDeg), aspect, m_zNear, m_zFar);
}

// FlyCamera

FlyCamera::FlyCamera(glm::vec3 pos,
        float yaw,
        float pitch,
        float fovDeg,
        float speed,
        float zNear,
        float zFar,
        float mouseSensitivity,
        bool lockCursor)
    : m_pos(pos)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_fovDeg(fovDeg)
    , m_speed(speed)
    , m_zNear(zNear)
    , m_zFar(zFar)
    , m_sensitivity(mouseSensitivity)
    , m_mode(lockCursor ? CursorMode::Locked : CursorMode::Free)
    , m_mouseInit(false)
    , m_lastX(0.0)
    , m_lastY(0.0)
{}

glm::vec3 FlyCamera::forwardDir() const
{
    const float yawRad = glm::radians(m_yaw);
    const float pitchRad = glm::radians(m_pitch);

    glm::vec3 dir;
    dir.x = std::cos(pitchRad) * std::cos(yawRad);
    dir.y = std::sin(pitchRad);
    dir.z = std::cos(pitchRad) * std::sin(yawRad);

    return glm::normalize(dir);
}

glm::mat4 FlyCamera::view() const
{
    const glm::vec3 f = forwardDir();
    return glm::lookAt(m_pos, m_pos + f, glm::vec3 WORLD_UP);
}

glm::mat4 FlyCamera::proj(float aspect) const
{
    return glm::perspective(glm::radians(m_fovDeg), aspect > 0.0f ? aspect : 1.0f, m_zNear, m_zFar);
}

void FlyCamera::applyCursorMode(GLFWwindow* win)
{
    if (m_mode == CursorMode::Locked)
    {
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }
    else
    {
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(win, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        }
    }

    m_mouseInit = false;
}

void FlyCamera::toggleCursorMode(GLFWwindow* win)
{
    if (m_mode == CursorMode::Locked)
    {
        m_mode = CursorMode::Free;
    }
    else
    {
        m_mode = CursorMode::Locked;
    }

    applyCursorMode(win);
}

void FlyCamera::update(GLFWwindow* win, float dt)
{
    // start state
    {
        const int want = (m_mode == CursorMode::Locked) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
        const int now  = glfwGetInputMode(win, GLFW_CURSOR);
        if (now != want)
        {
            applyCursorMode(win);
        }
    }

    // toggle cursor mode
    const int toggleState = glfwGetKey(win, GLFW_KEY_F1);
    if (toggleState == GLFW_PRESS && !m_prevTogglePressed)
    {
        toggleCursorMode(win);
    }

    m_prevTogglePressed = (toggleState == GLFW_PRESS);

    if (!glfwGetWindowAttrib(win, GLFW_HOVERED))
    {
        m_mouseInit = false;
        return;
    }

    // camera rotation
    if (m_mode == CursorMode::Locked)
    {
        double x;
        double y;
        glfwGetCursorPos(win, &x, &y);

        if (!m_mouseInit)
        {
            m_lastX = x;
            m_lastY = y;
            m_mouseInit = true;
        }

        const double dx = x - m_lastX;
        const double dy = m_lastY - y;
        m_lastX = x;
        m_lastY = y;

        m_yaw += static_cast<float>(dx) * m_sensitivity;
        m_pitch += static_cast<float>(dy) * m_sensitivity;

        if (m_pitch >  YAW_LIMIT)
        {
            m_pitch =  YAW_LIMIT;
        }
        if (m_pitch < -YAW_LIMIT)
        {
            m_pitch = -YAW_LIMIT;
        }

        if (m_yaw > 180.f)
        {
            m_yaw -= 360.f;
        }
        if (m_yaw < -180.f)
        {
            m_yaw += 360.f;
        }
    }

    // moving
    const glm::vec3 fwd = forwardDir();
    const glm::vec3 right = glm::normalize(glm::cross(fwd, glm::vec3 WORLD_UP));

    const bool boost = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
    const float step = m_speed * (boost ? 4.0f : 1.0f) * dt;

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_pos += fwd   * step;
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_pos -= fwd   * step;
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
    {
        m_pos -= right * step;
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
    {
        m_pos += right * step;
    }
}

} // namespace scene
} // namespace BulletRender
