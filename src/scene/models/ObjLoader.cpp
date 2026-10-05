/*
 * ObjLoader.cpp
 */

#include "ObjLoader.h"

#include "tiny_obj_loader.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace BulletRender {
namespace scene {

// squared length below this carries no direction
constexpr float LENGTH2_EPSILON = 1e-20f;

// obj numbers position, normal and uv apart, a vertex is the triplet of them
struct Corner {
    int position = -1;
    int normal = -1;
    int uv = -1;

    bool operator==(const Corner& other) const noexcept
    {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

struct CornerHash {
    size_t operator()(const Corner& corner) const noexcept
    {
        size_t hash = static_cast<size_t>(static_cast<uint32_t>(corner.position));
        hash = (hash * 31) ^ static_cast<size_t>(static_cast<uint32_t>(corner.normal));
        hash = (hash * 31) ^ static_cast<size_t>(static_cast<uint32_t>(corner.uv));
        return hash;
    }
};

static glm::vec3 safeNormalize(const glm::vec3& vector)
{
    const float length2 = glm::dot(vector, vector);

    if (length2 <= LENGTH2_EPSILON)
    {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }

    return vector * glm::inversesqrt(length2);
}

// file may carry no normals at all, then faces have to say which way they face
static void buildNormals(std::vector<Vertex>& vertices, const std::vector<unsigned>& indices)
{
    for (Vertex& vertex : vertices)
    {
        vertex.normal = glm::vec3(0.0f);
    }

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        Vertex& first = vertices[indices[i + 0]];
        Vertex& second = vertices[indices[i + 1]];
        Vertex& third = vertices[indices[i + 2]];

        // cross of two edges, longer for a wider face, so it weighs more where it meets others
        const glm::vec3 face = glm::cross(second.position - first.position, third.position - first.position);

        first.normal += face;
        second.normal += face;
        third.normal += face;
    }

    for (Vertex& vertex : vertices)
    {
        vertex.normal = safeNormalize(vertex.normal);
    }
}

std::vector<MeshData> ObjLoader::read(const std::string& path)
{
    tinyobj::ObjReaderConfig config;
    config.triangulate = true;
    config.vertex_color = false;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, config))
    {
        std::cerr << "obj load failed: " << path << " (" << reader.Error() << ")\n";
        return {};
    }

    if (!reader.Warning().empty())
    {
        std::cerr << "obj warning: " << reader.Warning() << "\n";
    }

    const tinyobj::attrib_t& attributes = reader.GetAttrib();

    const size_t positionCount = attributes.vertices.size() / 3;
    const size_t normalCount = attributes.normals.size() / 3;
    const size_t uvCount = attributes.texcoords.size() / 2;

    std::vector<MeshData> meshes;

    for (const tinyobj::shape_t& shape : reader.GetShapes())
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;
        std::unordered_map<Corner, unsigned, CornerHash> seen;

        bool hasNormals = true;

        for (const tinyobj::index_t& index : shape.mesh.indices)
        {
            const Corner corner{index.vertex_index, index.normal_index, index.texcoord_index};

            if (const auto it = seen.find(corner); it != seen.end())
            {
                indices.push_back(it->second);
                continue;
            }

            if (corner.position < 0 || static_cast<size_t>(corner.position) >= positionCount)
            {
                std::cerr << "obj vertex index out of range: " << path << "\n";
                return {};
            }

            Vertex vertex{};

            vertex.position = {
                attributes.vertices[3 * size_t(corner.position) + 0],
                attributes.vertices[3 * size_t(corner.position) + 1],
                attributes.vertices[3 * size_t(corner.position) + 2]
            };

            if (corner.normal >= 0 && static_cast<size_t>(corner.normal) < normalCount)
            {
                vertex.normal = {
                    attributes.normals[3 * size_t(corner.normal) + 0],
                    attributes.normals[3 * size_t(corner.normal) + 1],
                    attributes.normals[3 * size_t(corner.normal) + 2]
                };
            }
            else
            {
                hasNormals = false;
            }

            if (corner.uv >= 0 && static_cast<size_t>(corner.uv) < uvCount)
            {
                vertex.uv = {
                    attributes.texcoords[2 * size_t(corner.uv) + 0],
                    attributes.texcoords[2 * size_t(corner.uv) + 1]
                };
            }

            seen.emplace(corner, unsigned(vertices.size()));
            indices.push_back(unsigned(vertices.size()));
            vertices.push_back(vertex);
        }

        if (!hasNormals)
        {
            buildNormals(vertices, indices);
        }

        meshes.push_back({std::move(vertices), std::move(indices)});
    }

    return meshes;
}

} // namespace scene
} // namespace BulletRender
