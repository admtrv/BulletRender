/*
 * Shader.h
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace luchrender {
namespace render {

class Shader {
public:
    Shader(const std::string& vertPath, const std::string& fragPath)
    {
        loadFromFiles(vertPath, fragPath);
    }
    ~Shader();

    bool loadFromFiles(const std::string& vertPath, const std::string& fragPath);
    void bind() const;

    void setMat4(const char* name, const glm::mat4& mat) const;
    void setVec3(const char* name, const glm::vec3& vec) const;

private:
    unsigned m_id = 0;
    int uniformLoc(const char* name) const;
};

} // namespace render
} // namespace luchrender