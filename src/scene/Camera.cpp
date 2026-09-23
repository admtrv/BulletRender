/*
 * Camera.cpp
 */

#include "Camera.h"

#include "app/Window.h"
#include "utils/Input.h"

#include <cmath>


namespace BulletRender {
namespace scene {

constexpr float DRAG_SHARE = 0.0015f;       // share of view height per pixel dragged
constexpr float ZOOM_STEP = 0.1f;           // per notch of wheel

// Camera

// view matrix is lookAt(pos, target, worldUp) -> upper-left 3x3 is the camera basis as rows
glm::vec3 Camera::getForward() const
{
    glm::mat4 v = getView();
    return -glm::vec3(v[0][2], v[1][2], v[2][2]);
}

glm::vec3 Camera::getRight() const
{
    glm::mat4 v = getView();
    return glm::vec3(v[0][0], v[1][0], v[2][0]);
}

glm::vec3 Camera::getUp() const
{
    glm::mat4 v = getView();
    return glm::vec3(v[0][1], v[1][1], v[2][1]);
}

bool Camera::holdCursor(bool wanted)
{
    if (wanted != m_holding)
    {
        m_holding = wanted;

        app::Window::setCursorMode(wanted ? app::CursorMode::Captured : app::CursorMode::Normal);

        // cursor jumped on either change, next frame starts fresh delta
        m_cursorInit = false;
    }

    // something else may have freed cursor, put it back the way drag needs it
    if (m_holding && app::Window::getCursorMode() != app::CursorMode::Captured)
    {
        app::Window::setCursorMode(app::CursorMode::Captured);
        m_cursorInit = false;
    }

    return m_holding;
}

glm::vec2 Camera::cursorDelta()
{
    double x = 0.0;
    double y = 0.0;
    utils::Input::getCursorPos(x, y);

    const glm::vec2 delta = m_cursorInit ? glm::vec2(float(x - m_lastX), float(y - m_lastY)) : glm::vec2(0.0f);

    m_lastX = x;
    m_lastY = y;
    m_cursorInit = true;

    return delta;
}

glm::mat4 Camera::getProj(float aspect) const
{
    const float safe = aspect > 0.0f ? aspect : 1.0f;

    if (m_projection == Projection::Orthographic)
    {
        const float half = m_height * 0.5f;
        return glm::ortho(-half * safe, half * safe, -half, half, m_zNear, m_zFar);
    }

    return glm::perspective(glm::radians(m_fovDeg), safe, m_zNear, m_zFar);
}

// StaticCamera

glm::mat4 StaticCamera::getView() const
{
    return glm::lookAt(m_pos, m_target, m_up);
}

// FlyCamera

FlyCamera::FlyCamera(glm::vec3 pos,
        float yaw,
        float pitch,
        float fovDeg,
        float speed,
        float zNear,
        float zFar,
        float mouseSensitivity)
    : Camera("Fly Camera")
    , m_pos(pos)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_speed(speed)
    , m_sensitivity(mouseSensitivity)
{
    setFov(fovDeg);
    setClipPlanes(zNear, zFar);
}

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

glm::mat4 FlyCamera::getView() const
{
    const glm::vec3 f = forwardDir();
    return glm::lookAt(m_pos, m_pos + f, glm::vec3 WORLD_UP);
}

void FlyCamera::update(float dt)
{
    // holding right button looks around, releasing it hands cursor back
    if (holdCursor(utils::Input::isMouseDown(utils::MouseButton::Right)))
    {
        const glm::vec2 moved = cursorDelta() * m_sensitivity;

        m_yaw += moved.x;
        m_pitch -= moved.y;

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

    const bool boost = utils::Input::isKeyDown(utils::InputKey::LEFT_SHIFT) || utils::Input::isKeyDown(utils::InputKey::RIGHT_SHIFT);
    const float step = m_speed * (boost ? 4.0f : 1.0f) * dt;

    if (utils::Input::isKeyDown(utils::InputKey::W))
    {
        m_pos += fwd   * step;
    }
    if (utils::Input::isKeyDown(utils::InputKey::S))
    {
        m_pos -= fwd   * step;
    }
    if (utils::Input::isKeyDown(utils::InputKey::A))
    {
        m_pos -= right * step;
    }
    if (utils::Input::isKeyDown(utils::InputKey::D))
    {
        m_pos += right * step;
    }
}

// PanCamera

PanCamera::PanCamera(glm::vec2 center, float height, float depth, float speed)
    : Camera("Pan Camera"), m_center(center), m_depth(depth), m_speed(speed)
{
    setProjection(Projection::Orthographic);
    setHeight(height);

    // stands off plane, far side reaches well past whatever sits behind it
    setClipPlanes(0.1f, depth * 2.0f);
}

glm::mat4 PanCamera::getView() const
{
    const glm::vec3 eye{m_center, m_depth};
    return glm::lookAt(eye, glm::vec3(m_center, 0.0f), glm::vec3 WORLD_UP);
}

void PanCamera::update(float dt)
{
    // step scales with how much is on screen, so panning feels same at any zoom
    const float step = getHeight() * m_speed * dt;

    if (utils::Input::isKeyDown(utils::InputKey::W))
    {
        m_center.y += step;
    }
    if (utils::Input::isKeyDown(utils::InputKey::S))
    {
        m_center.y -= step;
    }
    if (utils::Input::isKeyDown(utils::InputKey::A))
    {
        m_center.x -= step;
    }
    if (utils::Input::isKeyDown(utils::InputKey::D))
    {
        m_center.x += step;
    }

    // holding right button drags view, same button fly camera looks with
    if (holdCursor(utils::Input::isMouseDown(utils::MouseButton::Right)))
    {
        // what a pixel spans, so grabbed point keeps up with cursor
        const glm::vec2 moved = cursorDelta() * getHeight() * DRAG_SHARE;

        m_center.x -= moved.x;
        m_center.y += moved.y;
    }

    if (const double scroll = utils::Input::consumeScrollDelta(); scroll != 0.0)
    {
        const float before = getHeight();
        setHeight(before * std::exp(float(-scroll) * ZOOM_STEP));

        // what sat under cursor stays there, so zooming closes in rather than drifts
        m_center += m_anchor * (before - getHeight()) * 0.5f;
    }
}

// OrbitCamera

OrbitCamera::OrbitCamera(glm::vec3 target, float radius, float fovDeg, float zNear, float zFar)
    : Camera("Orbit Camera"), m_target(target), m_radius(radius)
{
    setFov(fovDeg);
    setClipPlanes(zNear, zFar);
}

glm::vec3 OrbitCamera::getPosition() const
{
    float e = glm::clamp(m_elevation, 0.01f, 3.13159f);
    return m_target + glm::vec3(
        m_radius * std::sin(e) * std::cos(m_azimuth),
        m_radius * std::cos(e),
        m_radius * std::sin(e) * std::sin(m_azimuth)
    );
}

void OrbitCamera::setPosition(const glm::vec3& pos)
{
    // orbit itself unchanged, whole rig slides so eye lands on pos
    m_target += pos - getPosition();
}

glm::mat4 OrbitCamera::getView() const
{
    return glm::lookAt(getPosition(), m_target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void OrbitCamera::update(float /*dt*/)
{
    double x = 0.0;
    double y = 0.0;
    utils::Input::getCursorPos(x, y);

    const bool dragging = utils::Input::isMouseDown(utils::MouseButton::Left);

    if (dragging)
    {
        m_azimuth   += static_cast<float>(x - m_lastX) * 0.005f;
        m_elevation -= static_cast<float>(y - m_lastY) * 0.005f;
        m_elevation = glm::clamp(m_elevation, 0.01f, 3.13159f);
    }

    m_lastX = x;
    m_lastY = y;

    double scroll = utils::Input::consumeScrollDelta();
    if (scroll != 0.0)
    {
        m_radius *= static_cast<float>(std::pow(0.9, scroll));
    }

    m_moving = dragging || scroll != 0.0;
}

} // namespace scene
} // namespace BulletRender
