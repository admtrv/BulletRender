/*
 * camera.cpp
 */

#include "camera.h"

namespace luchrender {
namespace scene {

FlyCamera::FlyCamera(glm::vec3 pos, float yaw, float pitch, float fovDeg, float speed, float zNear, float zFar):
    m_pos(pos), m_yaw(yaw), m_pitch(pitch), m_fovDeg(fovDeg), m_speed(speed), m_zNear(zNear), m_zFar(zFar) {}

glm::mat4 FlyCamera::view() const
{
    float cy = std::cos(glm::radians(m_yaw));
    float sy = std::sin(glm::radians(m_yaw));
    float cp = std::cos(glm::radians(m_pitch));
    float sp = std::sin(glm::radians(m_pitch));
    glm::vec3 f = glm::normalize(glm::vec3(cy*cp, sp, sy*cp));
    return glm::lookAt(m_pos, m_pos + f, {0,1,0});
}

glm::mat4 FlyCamera::proj(float aspect) const
{
    return glm::perspective(glm::radians(m_fovDeg),aspect > 0.0f ? aspect : 1.0f, m_zNear, m_zFar);
}

void FlyCamera::update(GLFWwindow* win, float dt)
{
    // view direction
    float cy = std::cos(glm::radians(m_yaw));
    float sy = std::sin(glm::radians(m_yaw));
    float cp = std::cos(glm::radians(m_pitch));
    float sp = std::sin(glm::radians(m_pitch));
    glm::vec3 forward = glm::normalize(glm::vec3(cy*cp, sp, sy*cp));

    // straight movement
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_pos += forward * m_speed * dt;
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_pos -= forward * m_speed * dt;
    }

    // yaw turns
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS or glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        m_yaw -= 30.f * dt; // right
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS or glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        m_yaw += 30.f * dt; // left
    }

    // pitch turns
    if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
    {
        m_pitch += 30.f * dt;
        if (m_pitch > 89.f) m_pitch = 89.f;
    }
    if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        m_pitch -= 30.f * dt;
        if (m_pitch < -89.f) m_pitch = -89.f;
    }
}

} // namespace scene
} // namespace luchrender