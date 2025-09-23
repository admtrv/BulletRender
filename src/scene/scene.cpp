/*
 * scene.cpp
 */

#include "scene.h"

namespace luchrender {
namespace scene {

SceneObject* Scene::addObject(Model* model)
{
    m_objects.emplace_back(std::make_unique<SceneObject>(model));
    return m_objects.back().get();
}

void Scene::removeObject(size_t index)
{
    if (index < m_objects.size()) {
        m_objects.erase(m_objects.begin() + index);
    }
}

void Scene::clear()
{
    m_objects.clear();
}

} // namespace scene
} // namespace luchrender