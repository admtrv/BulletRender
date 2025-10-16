/*
 * Scene.h
 */

#pragma once

#include "Model.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "render/Material.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace BulletRender {
namespace scene {

class SceneObject {
public:
    SceneObject(Model* model = nullptr) : m_model(model) {}

    void setModel(Model* model) { m_model = model; }
    Model* getModel() const { return m_model; }

    Transform& getTransform() { return m_transform; }
    const Transform& getTransform() const { return m_transform; }

    render::Material& getMaterial() { return m_material; }
    const render::Material& getMaterial() const { return m_material; }

private:
    Model* m_model;
    Transform m_transform;
    render::Material m_material;
};

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    SceneObject* addObject(Model* model);
    void removeObject(size_t index);
    const std::vector<std::unique_ptr<SceneObject>>& getObjects() const { return m_objects; };
    void clear();

    void setCamera(const Camera* cam) { m_camera = cam; }
    void setLight(const Light* light) { m_light = light; }
    void setAspect(float aspect) { m_aspect = aspect; }

    const Camera* getCamera() const { return m_camera; }
    const Light* getLight() const { return m_light; }
    float getAspect() const { return m_aspect; }

private:
    std::vector<std::unique_ptr<SceneObject>> m_objects;

    const Camera* m_camera = nullptr;
    const Light* m_light = nullptr;

    float m_aspect = 1.0f;
};

} // namespace scene
} // namespace BulletRender