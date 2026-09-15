/*
 * Light.cpp
 */

#include "Light.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace BulletRender {
namespace scene {

// dot product above this means direction is collinear with up
constexpr float COLLINEAR_DOT = 0.999f;

// Light

void Light::setDirection(const glm::vec3& direction)
{
    const float length = glm::length(direction);

    // a zero vector carries no direction, the previous rotation stays
    if (length < glm::epsilon<float>())
    {
        return;
    }

    m_transform.setLocalRotation(glm::quatLookAt(direction / length, glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Light::setColor(const glm::vec3& c)
{
    m_color = c;
}

glm::vec3 Light::getColor() const
{
    return m_color;
}

void Light::setIntensity(float i)
{
    m_intensity = i;
}

float Light::getIntensity() const
{
    return m_intensity;
}

void Light::setCastsShadow(bool v)
{
    m_castsShadow = v;
}

bool Light::getCastsShadow() const
{
    return m_castsShadow;
}

// DirectionalLight

DirectionalLight::DirectionalLight(glm::vec3 dir) : Light("Directional Light")
{
    setDirection(dir);
    m_castsShadow = true;
}

void DirectionalLight::setShadowOrthoSize(float halfSize)
{
    m_orthoHalfSize = halfSize;
}

float DirectionalLight::getShadowOrthoSize() const
{
    return m_orthoHalfSize;
}

void DirectionalLight::setShadowTarget(const glm::vec3& t)
{
    m_shadowTarget = t;
}

glm::vec3 DirectionalLight::getShadowTarget() const
{
    return m_shadowTarget;
}

glm::mat4 DirectionalLight::getViewProj() const
{
    // place virtual camera back along the light direction, looking at the target
    float dist = m_orthoHalfSize * 2.0f;
    const glm::vec3 direction = getDirection();
    glm::vec3 eye = m_shadowTarget + direction * dist;

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(direction, up)) > COLLINEAR_DOT)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    glm::mat4 view = glm::lookAt(eye, m_shadowTarget, up);
    glm::mat4 proj = glm::ortho(-m_orthoHalfSize, m_orthoHalfSize,
                                -m_orthoHalfSize, m_orthoHalfSize,
                                0.1f, dist * 2.0f);
    return proj * view;
}

// PointLight

PointLight::PointLight(glm::vec3 pos, float range) : Light("Point Light"), m_range(range)
{
    setPosition(pos);
}

void PointLight::setRange(float r)
{
    m_range = r;
}

float PointLight::getRange() const
{
    return m_range;
}

// SpotLight

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, float innerDeg, float outerDeg, float range)
    : Light("Spot Light"),
      m_innerCos(glm::cos(glm::radians(innerDeg))),
      m_outerCos(glm::cos(glm::radians(outerDeg))),
      m_range(range)
{
    setPosition(pos);
    setDirection(dir);

    m_castsShadow = true;
}

void SpotLight::setCones(float innerDeg, float outerDeg)
{
    m_innerCos = glm::cos(glm::radians(innerDeg));
    m_outerCos = glm::cos(glm::radians(outerDeg));
}

float SpotLight::getInnerCos() const
{
    return m_innerCos;
}

float SpotLight::getOuterCos() const
{
    return m_outerCos;
}

void SpotLight::setRange(float r)
{
    m_range = r;
}

float SpotLight::getRange() const
{
    return m_range;
}

glm::mat4 SpotLight::getViewProj() const
{
    // fov from outer cone angle (cos -> angle, doubled because cone is half-angle)
    float outerAngle = glm::acos(glm::clamp(m_outerCos, -1.0f, 1.0f));
    float fov = outerAngle * 2.0f;

    const glm::vec3 eye = getPosition();
    const glm::vec3 direction = getDirection();

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(direction, up)) > COLLINEAR_DOT)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    glm::mat4 view = glm::lookAt(eye, eye + direction, up);
    glm::mat4 proj = glm::perspective(fov, 1.0f, 0.1f, m_range);
    return proj * view;
}

} // namespace scene
} // namespace BulletRender
