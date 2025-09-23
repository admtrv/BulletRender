/*
 * model.cpp
 */

#include "model.h"

namespace luchrender {
namespace scene {

// key for unique vertex - pair of position and normal indices
struct VNKey {
    int vertexIdx = -1; // vertex index
    int normalIdx = -1; // normal index

    bool operator==(const VNKey& other) const noexcept
    {
        return vertexIdx == other.vertexIdx && normalIdx == other.normalIdx;
    }
};

struct VNKeyHash {

    size_t operator()(const VNKey& key) const noexcept
    {
        return (static_cast<size_t>(static_cast<uint32_t>(key.vertexIdx)) << 32) ^ static_cast<uint32_t>(key.normalIdx);
    }
};


static glm::vec3 safeNormalize(const glm::vec3& vector) {
    float len2 = glm::dot(vector, vector);

    if (len2 <= 1e-20f)
    {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }

    return vector * glm::inversesqrt(len2);
}

bool Model::loadObj(const std::string& path)
{
    m_meshes.clear();

    tinyobj::ObjReaderConfig config;
    config.triangulate = true;
    config.vertex_color = false;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "tinyobj error: " << reader.Error() << "\n";
        }

        return false;
    }

    if (!reader.Warning().empty())
    {
        std::cerr << "tinyobj warn: " << reader.Warning() << "\n";
    }

    const tinyobj::attrib_t& attrib = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();

    const size_t positionCount = attrib.vertices.size() / 3;
    const size_t normalCount = attrib.normals.size()  / 3;

    // create a separate mesh per shape
    m_meshes.reserve(shapes.size());

    for (const auto& shape : shapes)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        vertices.reserve(shape.mesh.indices.size());
        indices.reserve(shape.mesh.indices.size());

        std::unordered_map<VNKey, unsigned, VNKeyHash> uniqueMap;
        uniqueMap.reserve(shape.mesh.indices.size());

        bool needRecomputeNormals = (normalCount == 0); // no normals in obj

        // unpack shape indices into unique vertices
        for (const tinyobj::index_t& faceIdx : shape.mesh.indices)
        {
            VNKey key{ faceIdx.vertex_index, faceIdx.normal_index };

            auto it = uniqueMap.find(key);

            if (it == uniqueMap.end())
            {
                if (faceIdx.vertex_index < 0 || static_cast<size_t>(faceIdx.vertex_index) >= positionCount)
                {
                    std::cerr << "obj vertex index out of range\n";
                    return false;
                }

                glm::vec3 position = {
                    attrib.vertices[3 * static_cast<size_t>(faceIdx.vertex_index) + 0],
                    attrib.vertices[3 * static_cast<size_t>(faceIdx.vertex_index) + 1],
                    attrib.vertices[3 * static_cast<size_t>(faceIdx.vertex_index) + 2]
                };

                glm::vec3 normal(0.0f);

                if (faceIdx.normal_index >= 0 && static_cast<size_t>(faceIdx.normal_index) < normalCount)
                {
                    normal = {
                        attrib.normals[3 * static_cast<size_t>(faceIdx.normal_index) + 0],
                        attrib.normals[3 * static_cast<size_t>(faceIdx.normal_index) + 1],
                        attrib.normals[3 * static_cast<size_t>(faceIdx.normal_index) + 2]
                    };
                }
                else
                {
                    needRecomputeNormals = true;
                }

                unsigned NewVertexIdx = static_cast<unsigned>(vertices.size());
                uniqueMap.emplace(key, NewVertexIdx);
                vertices.push_back(Vertex{ position, normal });
                indices.push_back(NewVertexIdx);
            }
            else
            {
                indices.push_back(it->second);
            }
        }

        // if normals are missing globally or per-vertex, compute smoothed vertex normals
        if (needRecomputeNormals) {
            for (auto& vertex : vertices)
            {
                vertex.normal = glm::vec3(0.0f);
            }

            if (indices.size() % 3 != 0)
            {
                std::cerr << "obj not triangulated as expected\n";
                return false;
            }

            for (size_t i = 0; i < indices.size(); i += 3)
            {
                Vertex& vertexA = vertices[indices[i + 0]];
                Vertex& vertexB = vertices[indices[i + 1]];
                Vertex& vertexC = vertices[indices[i + 2]];

                glm::vec3 edge1 = vertexB.position - vertexA.position;
                glm::vec3 edge2 = vertexC.position - vertexA.position;

                glm::vec3 faceNormal = glm::cross(edge1, edge2); // unnormalized face normal

                vertexA.normal += faceNormal;
                vertexB.normal += faceNormal;
                vertexC.normal += faceNormal;
            }

            // normalize accumulated vertex normals
            for (auto& vertex : vertices)
            {
                vertex.normal = safeNormalize(vertex.normal);
            }
        }

        // make GPU mesh
        m_meshes.emplace_back(vertices, indices);
    }

    return true;
}

} // namespace scene
} // namespace luchrender