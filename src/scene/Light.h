/*
 * Light.h
 */

#pragma once

#include "Named.h"
#include "Transform.h"

#include <glm/glm.hpp>

namespace BulletRender {
namespace scene {

enum class LightType {
    Ambient,
    Directional,
    Point,
    Spot
};

// base light
class Light : public Named {
public:
    explicit Light(std::string name = "Light") : Named(std::move(name)) {}
    virtual ~Light() = default;
    virtual LightType getType() const = 0;

    // pose, a light stands and points the way its transform does
    Transform& getTransform() { return m_transform; }
    const Transform& getTransform() const { return m_transform; }

    glm::vec3 getPosition() const { return m_transform.getPosition(); }
    void setPosition(const glm::vec3& position) { m_transform.setLocalPosition(position); }

    glm::vec3 getDirection() const { return m_transform.getForward(); }
    void setDirection(const glm::vec3& direction);

    void setColor(const glm::vec3& c);
    glm::vec3 getColor() const;

    void setIntensity(float i);
    float getIntensity() const;

    void setCastsShadow(bool v);
    bool getCastsShadow() const;

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

protected:
    Transform m_transform;

    glm::vec3 m_color = glm::vec3(1.0f);
    float m_intensity = 1.0f;
    bool m_castsShadow = false;
    bool m_visible = true;
};

// global fill light
class AmbientLight : public Light {
public:
    AmbientLight() : Light("Ambient Light") {}

    LightType getType() const override { return LightType::Ambient; }
};

// parallel rays from infinity, like sun
class DirectionalLight : public Light {
public:
    explicit DirectionalLight(glm::vec3 dir = glm::vec3(-0.4f, 1.0f, 0.2f));

    // shadow frustum size and target for orthographic shadow projection
    void setShadowOrthoSize(float halfSize);
    float getShadowOrthoSize() const;

    void setShadowTarget(const glm::vec3& t);
    glm::vec3 getShadowTarget() const;

    glm::mat4 getViewProj() const;

    LightType getType() const override { return LightType::Directional; }

private:
    float m_orthoHalfSize = 15.0f;
    glm::vec3 m_shadowTarget = glm::vec3(0.0f);
};

// omnidirectional point source with distance falloff
class PointLight : public Light {
public:
    explicit PointLight(glm::vec3 pos = glm::vec3(0.0f), float range = 20.0f);

    void setRange(float r);
    float getRange() const;

    LightType getType() const override { return LightType::Point; }

private:
    float m_range;
};

// cone-restricted point source, like a flashlight
class SpotLight : public Light {
public:
    SpotLight(glm::vec3 pos = glm::vec3(0.0f),
              glm::vec3 dir = glm::vec3(0.0f, -1.0f, 0.0f),
              float innerDeg = 15.0f,
              float outerDeg = 25.0f,
              float range = 30.0f);

    // angles stored as cosines
    void setCones(float innerDeg, float outerDeg);
    float getInnerCos() const;
    float getOuterCos() const;

    void setRange(float r);
    float getRange() const;

    glm::mat4 getViewProj() const;

    LightType getType() const override { return LightType::Spot; }

private:
    float m_innerCos;
    float m_outerCos;
    float m_range;
};

} // namespace scene
} // namespace BulletRender
