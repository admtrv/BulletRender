/*
 * Model.cpp
 */

#include "Model.h"

#include <glm/gtc/constants.hpp>

#include <cmath>

// squared length below this carries no direction
constexpr float LENGTH2_EPSILON = 1e-20f;

namespace BulletRender {
namespace scene {

// normal map is read along u, which uv layout over triangle decides
static void buildTangents(std::vector<Vertex>& vertices, const std::vector<unsigned>& indices)
{
    std::vector<glm::vec3> along(vertices.size(), glm::vec3(0.0f));
    std::vector<glm::vec3> across(vertices.size(), glm::vec3(0.0f));

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vertex& first = vertices[indices[i + 0]];
        const Vertex& second = vertices[indices[i + 1]];
        const Vertex& third = vertices[indices[i + 2]];

        const glm::vec3 edge1 = second.position - first.position;
        const glm::vec3 edge2 = third.position - first.position;

        const glm::vec2 uv1 = second.uv - first.uv;
        const glm::vec2 uv2 = third.uv - first.uv;

        const float determinant = uv1.x * uv2.y - uv2.x * uv1.y;

        // uv collapsed to a line or a point says nothing about direction
        if (std::abs(determinant) < 1e-12f)
        {
            continue;
        }

        const float scale = 1.0f / determinant;

        const glm::vec3 tangent = (edge1 * uv2.y - edge2 * uv1.y) * scale;
        const glm::vec3 bitangent = (edge2 * uv1.x - edge1 * uv2.x) * scale;

        for (size_t corner = 0; corner < 3; corner++)
        {
            along[indices[i + corner]] += tangent;
            across[indices[i + corner]] += bitangent;
        }
    }

    for (size_t i = 0; i < vertices.size(); i++)
    {
        const glm::vec3 normal = vertices[i].normal;

        // gram-schmidt, what is left of the tangent once it stops leaning on the normal
        glm::vec3 tangent = along[i] - normal * glm::dot(normal, along[i]);

        if (glm::dot(tangent, tangent) <= LENGTH2_EPSILON)
        {
            tangent = std::abs(normal.x) < 0.9f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
            tangent -= normal * glm::dot(normal, tangent);
        }

        tangent = glm::normalize(tangent);

        // mirrored uv turns the third axis round, the sign carries that to the shader
        const float sign = glm::dot(glm::cross(normal, tangent), across[i]) < 0.0f ? -1.0f : 1.0f;

        vertices[i].tangent = glm::vec4(tangent, sign);
    }
}

void Model::addMesh(std::vector<Vertex> vertices, const std::vector<unsigned>& indices)
{
    buildTangents(vertices, indices);

    // first mesh seeds bounds, later ones only stretch them
    if (m_meshes.empty() && !vertices.empty())
    {
        m_boundsMin = vertices.front().position;
        m_boundsMax = vertices.front().position;
    }

    for (const Vertex& vertex : vertices)
    {
        m_boundsMin = glm::min(m_boundsMin, vertex.position);
        m_boundsMax = glm::max(m_boundsMax, vertex.position);
    }

    m_meshes.emplace_back(vertices, indices);
}

void Model::clearMeshes()
{
    m_meshes.clear();

    m_boundsMin = glm::vec3(0.0f);
    m_boundsMax = glm::vec3(0.0f);
}

// Box

Box::Box() : Box(1.0f, 1.0f, 1.0f) {}

Box::Box(float sizeX, float sizeY, float sizeZ)
{
    float hx = sizeX * 0.5f;
    float hy = sizeY * 0.5f;
    float hz = sizeZ * 0.5f;

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    // front face (z+)
    vertices.push_back({{-hx, -hy,  hz}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ hx, -hy,  hz}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ hx,  hy,  hz}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-hx,  hy,  hz}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}});

    // back face (z-)
    vertices.push_back({{ hx, -hy, -hz}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{-hx, -hy, -hz}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{-hx,  hy, -hz}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{ hx,  hy, -hz}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}});

    // right face (x+)
    vertices.push_back({{ hx, -hy,  hz}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ hx, -hy, -hz}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ hx,  hy, -hz}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{ hx,  hy,  hz}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}});

    // left face (x-)
    vertices.push_back({{-hx, -hy, -hz}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{-hx, -hy,  hz}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{-hx,  hy,  hz}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-hx,  hy, -hz}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}});

    // top face (y+)
    vertices.push_back({{-hx,  hy,  hz}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ hx,  hy,  hz}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ hx,  hy, -hz}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-hx,  hy, -hz}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}});

    // bottom face (y-)
    vertices.push_back({{-hx, -hy, -hz}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ hx, -hy, -hz}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ hx, -hy,  hz}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-hx, -hy,  hz}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}});

    for (unsigned face = 0; face < 6; face++)
    {
        unsigned base = face * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);

        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    addMesh(vertices, indices);
}

unsigned Model::getVertexCount() const
{
    unsigned count = 0;
    for (const Mesh& mesh : m_meshes)
    {
        count += mesh.getVertexCount();
    }
    return count;
}

unsigned Model::getTriangleCount() const
{
    unsigned count = 0;
    for (const Mesh& mesh : m_meshes)
    {
        count += mesh.getTriangleCount();
    }
    return count;
}

Sphere::Sphere() : Sphere(0.5f, 32, 16) {}

Sphere::Sphere(float radius, int segments, int rings)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    // uv sphere, ring 0 is north pole and ring "rings" south one
    for (int ring = 0; ring <= rings; ring++)
    {
        const float v = static_cast<float>(ring) / static_cast<float>(rings);
        const float phi = v * glm::pi<float>();

        for (int segment = 0; segment <= segments; segment++)
        {
            const float u = static_cast<float>(segment) / static_cast<float>(segments);
            const float theta = u * glm::two_pi<float>();

            // unit sphere point doubles as normal
            const glm::vec3 normal = {
                std::sin(phi) * std::cos(theta),
                std::cos(phi),
                std::sin(phi) * std::sin(theta)
            };

            vertices.push_back({normal * radius, normal, {u, 1.0f - v}});
        }
    }

    const int stride = segments + 1;
    for (int ring = 0; ring < rings; ring++)
    {
        for (int segment = 0; segment < segments; segment++)
        {
            const unsigned current = static_cast<unsigned>(ring * stride + segment);
            const unsigned next = static_cast<unsigned>(current + stride);

            indices.push_back(current);
            indices.push_back(current + 1);
            indices.push_back(next);

            indices.push_back(current + 1);
            indices.push_back(next + 1);
            indices.push_back(next);
        }
    }

    addMesh(vertices, indices);
}

Quad::Quad() : Quad(1.0f, 1.0f) {}

Quad::Quad(float sizeX, float sizeY)
{
    const float hx = sizeX * 0.5f;
    const float hy = sizeY * 0.5f;

    const std::vector<Vertex> vertices = {
        {{-hx, -hy, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ hx, -hy, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ hx,  hy, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-hx,  hy, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
    };

    addMesh(vertices, {0, 1, 2, 0, 2, 3});
}

Circle::Circle() : Circle(0.5f, 32) {}

Circle::Circle(float radius, int segments)
{
    segments = std::max(segments, 3);

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    // middle first, rim fans out of it
    vertices.push_back({{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f}});

    for (int segment = 0; segment <= segments; segment++)
    {
        const float angle = 2.0f * glm::pi<float>() * float(segment) / float(segments);
        const float x = std::cos(angle);
        const float y = std::sin(angle);

        vertices.push_back({{x * radius, y * radius, 0.0f}, {0.0f, 0.0f, 1.0f}, {x * 0.5f + 0.5f, y * 0.5f + 0.5f}});
    }

    for (int segment = 0; segment < segments; segment++)
    {
        indices.push_back(0);
        indices.push_back(static_cast<unsigned>(segment + 1));
        indices.push_back(static_cast<unsigned>(segment + 2));
    }

    addMesh(vertices, indices);
}

} // namespace scene
} // namespace BulletRender
