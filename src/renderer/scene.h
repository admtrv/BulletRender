/*
 * scene.h
 */
#pragma once

#include "model.h"
#include "camera/camera.h"
#include "light/light.h"

#include <glm/glm.hpp>

#include <vector>

namespace renderer {

struct SceneObject {
    const Model* model = nullptr;
    glm::mat4 transform{1.0f};
};

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void addObject(const Model* model, const glm::mat4& transform);
    void removeObject(size_t index);
    void clear();

    const std::vector<SceneObject>& getObjects() const { return m_objects; }

    void setCamera(const camera::Camera* cam) { m_camera = cam; }
    void setLight(const light::Light* light) { m_light = light; }
    void setAspect(float aspect) { m_aspect = aspect; }

    const camera::Camera* getCamera() const { return m_camera; }
    const light::Light* getLight() const { return m_light; }
    float getAspect() const { return m_aspect; }

private:
    std::vector<SceneObject> m_objects;

    const camera::Camera* m_camera = nullptr;
    const light::Light* m_light = nullptr;

    float m_aspect = 1.0f;
};

} // namespace renderer
