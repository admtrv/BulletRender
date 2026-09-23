/*
* Camera.h
 */

#pragma once

#include "Named.h"

#include "app/Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace BulletRender {
namespace scene {

#define YAW_LIMIT   89.0f
#define WORLD_UP    {0.0f, 1.0f, 0.0f}

enum class CameraType {
    Static,
    Fly,
    Orbit,
    Pan
};

// how camera flattens world, perspective keeps depth, orthographic drops it
enum class Projection {
    Perspective,
    Orthographic
};

// basic camera
class Camera : public Named {
public:
    explicit Camera(std::string name = "Camera") : Named(std::move(name)) {}
    virtual ~Camera() = default;

    virtual CameraType getType() const = 0;

    virtual glm::mat4 getView() const = 0;
    virtual glm::vec3 getPosition() const = 0;
    virtual void setPosition(const glm::vec3& pos) = 0;

    // projection
    glm::mat4 getProj(float aspect) const;

    Projection getProjection() const { return m_projection; }
    void setProjection(Projection projection) { m_projection = projection; }

    float getNear() const { return m_zNear; }
    float getFar() const { return m_zFar; }
    void setClipPlanes(float zNear, float zFar) { m_zNear = zNear; m_zFar = zFar; }

    float getFov() const { return m_fovDeg; }                       // vertical angle, perspective only
    void setFov(float fovDeg) { m_fovDeg = fovDeg; }

    float getHeight() const { return m_height; }                    // world units view spans, orthographic only
    void setHeight(float height) { m_height = height; }

    // orientation basis derived from getView() (transpose of upper 3x3)
    virtual glm::vec3 getForward() const;
    virtual glm::vec3 getRight() const;
    virtual glm::vec3 getUp() const;

    virtual void update(float dt) {}

protected:
    // hides and locks cursor while button drags view, says whether drag carries on
    bool holdCursor(bool wanted);

    // cursor moved since last frame, zero on first one so view never jumps
    glm::vec2 cursorDelta();

    Projection m_projection = Projection::Perspective;

    float m_fovDeg = 60.0f;
    float m_height = 10.0f;
    float m_zNear = 0.1f;
    float m_zFar = 100.0f;

private:
    bool m_holding = false;
    bool m_cursorInit = false;
    double m_lastX = 0.0;
    double m_lastY = 0.0;
};

// static camera (position ->->-> target)
class StaticCamera : public Camera {
public:
    StaticCamera(const glm::vec3& pos,
            const glm::vec3& target,
            const glm::vec3& up = {0.0f, 1.0f, 0.0f},
            float fovDeg = 60.0f,
            float zNear = 0.1f,
            float zFar = 100.0f)
        : Camera("Static Camera"), m_pos(pos), m_target(target), m_up(up)
    {
        setFov(fovDeg);
        setClipPlanes(zNear, zFar);
    }

    CameraType getType() const override { return CameraType::Static; }

    glm::mat4 getView() const override;
    glm::vec3 getPosition() const override { return m_pos; }
    void setPosition(const glm::vec3& pos) override { m_pos = pos; }

    void setTarget(const glm::vec3& target) { m_target = target; }
    glm::vec3 getTarget() const { return m_target; }

private:
    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::vec3 m_up;
};

// fly camera (Look: hold RMB, Move: WASD, Boost: Shift)
class FlyCamera : public Camera {
public:
    explicit FlyCamera(glm::vec3 pos = {0,1,5},
                       float yaw = -90.f,
                       float pitch = 0.f,
                       float fovDeg = 60.f,
                       float speed = 3.f,
                       float zNear = 0.1f,
                       float zFar = 100.f,
                       float mouseSensitivity = 0.1f);

    CameraType getType() const override { return CameraType::Fly; }

    glm::mat4 getView() const override;
    glm::vec3 getPosition() const override { return m_pos; }
    void setPosition(const glm::vec3& pos) override { m_pos = pos; }

    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }

    void update(float dt) override;

private:
    glm::vec3 forwardDir() const;

    glm::vec3 m_pos;
    float m_yaw;
    float m_pitch;
    float m_speed;
    float m_sensitivity;
};

// pan camera (Move: WASD or hold RMB, Zoom: wheel)
// looks straight down depth axis, nothing turns it so what it shows stays flat
class PanCamera : public Camera {
public:
    explicit PanCamera(glm::vec2 center = {0.0f, 0.0f}, float height = 10.0f, float depth = 10.0f, float speed = 0.8f);

    CameraType getType() const override { return CameraType::Pan; }

    glm::mat4 getView() const override;
    glm::vec3 getPosition() const override { return {m_center, m_depth}; }
    void setPosition(const glm::vec3& pos) override { m_center = {pos.x, pos.y}; m_depth = pos.z; }

    float getSpeed() const { return m_speed; }                      // share of view height crossed per second
    void setSpeed(float speed) { m_speed = speed; }

    void update(float dt) override;

    // where wheel pulls towards, in view units from middle, keeps a spot under cursor
    void setZoomAnchor(const glm::vec2& anchor) { m_anchor = anchor; }

private:
    glm::vec2 m_center;     // what sits in middle of view
    glm::vec2 m_anchor{0.0f, 0.0f};
    float m_depth;          // how far back it stands, so nothing falls behind it
    float m_speed;
};

// orbit camera (LMB drag = rotate, scroll = zoom)
class OrbitCamera : public Camera {
public:
    explicit OrbitCamera(glm::vec3 target = {0, 0, 0},
                         float radius = 10.0f,
                         float fovDeg = 60.0f,
                         float zNear = 0.1f,
                         float zFar = 1000.0f);

    CameraType getType() const override { return CameraType::Orbit; }

    glm::mat4 getView() const override;
    glm::vec3 getPosition() const override;
    void setPosition(const glm::vec3& pos) override;

    glm::vec3 getTarget() const { return m_target; }
    void setTarget(const glm::vec3& target) { m_target = target; }

    float getRadius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }
    bool isMoving() const { return m_moving; }

    void update(float dt) override;

private:
    glm::vec3 m_target;
    float m_radius;
    float m_azimuth = 0.0f;
    float m_elevation = 1.5707963f;

    double m_lastX = 0.0;
    double m_lastY = 0.0;
    bool m_moving = false;
};

} // namespace scene
} // namespace BulletRender
