/*
 * scene.cpp
 */

#include "scene.h"

namespace scene {

void Scene::addObject(const Model* model, const glm::mat4& transform)
{
    m_objects.push_back({model, transform});
}

void Scene::removeObject(size_t index)
{
    if (index < m_objects.size())
    {
        m_objects.erase(m_objects.begin() + index);
    }
}

void Scene::clear()
{
    m_objects.clear();
}

} // namespace scene
