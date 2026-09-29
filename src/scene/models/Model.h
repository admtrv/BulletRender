/*
 * Model.h
 */

#pragma once

#include "Mesh.h"

#include <glm/glm.hpp>

#include <vector>

namespace BulletRender {
namespace scene {

class Model {
public:
    Model() = default;
    virtual ~Model() = default;

    // geometry
    const std::vector<Mesh>& getMeshes() const { return m_meshes; }
    void addMesh(std::vector<Vertex> vertices, const std::vector<unsigned>& indices);    // builds tangents, so every source gets them alike
    void clearMeshes();

    // counts
    unsigned getVertexCount() const;
    unsigned getTriangleCount() const;

    // bounds, axis aligned in model space, zero sized when empty
    const glm::vec3& getBoundsMin() const { return m_boundsMin; }
    const glm::vec3& getBoundsMax() const { return m_boundsMax; }

protected:
    std::vector<Mesh> m_meshes;

    glm::vec3 m_boundsMin{0.0f};
    glm::vec3 m_boundsMax{0.0f};
};

// primitives, spelled out rather than read from a file

class Box : public Model {
public:
    Box();
    Box(float sizeX, float sizeY, float sizeZ);
};

class Sphere : public Model {
public:
    Sphere();
    Sphere(float radius, int segments, int rings);
};

// flat ones lie in xy, what sprite is drawn on

class Quad : public Model {
public:
    Quad();
    Quad(float sizeX, float sizeY);
};

class Circle : public Model {
public:
    Circle();
    Circle(float radius, int segments);
};

} // namespace scene
} // namespace BulletRender
