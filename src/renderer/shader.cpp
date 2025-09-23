/*
 * shader.cpp
 */

#include "shader.h"

namespace {

std::string readFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        return {};
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// compile shader of type
unsigned compile(GLenum type, const char* src)
{
    unsigned shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

    if (!ok)
    {
        int size = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);

        std::string log(size, '\0');
        glGetShaderInfoLog(shader, size, nullptr, log.data());

        std::cerr << "shader compile error: " << log << "\n";
    }

    return shader;
}

} // namespace

namespace renderer {

bool Shader::loadFromFiles(const std::string& vertPath, const std::string& fragPath)
{
    std::string vertString = readFile(vertPath);
    std::string fragString = readFile(fragPath);

    if (vertString.empty() || fragString.empty())
    {
        std::cerr << "cannot read " << vertPath << " or " << fragPath << "\n";
        return false;
    }

    unsigned vertexShader = compile(GL_VERTEX_SHADER,   vertString.c_str());
    unsigned fragmentShader = compile(GL_FRAGMENT_SHADER, fragString.c_str());

    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    int ok = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &ok);

    if (!ok) {

        int size = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &size);

        std::string log(size, '\0');
        glGetProgramInfoLog(m_id, size, nullptr, log.data());

        std::cerr << "program link error: " << log << "\n";

        glDeleteProgram(m_id);
        m_id = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return m_id != 0;
}

Shader::~Shader()
{
    if (m_id)
    {
        glDeleteProgram(m_id);
    }
}

void Shader::bind() const
{
    glUseProgram(m_id);
}

int Shader::uniformLoc(const char* name) const
{
    return glGetUniformLocation(m_id, name);
}

void Shader::setMat4(const char* name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(uniformLoc(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const char* name, const glm::vec3& vec) const
{
    glUniform3fv(uniformLoc(name), 1, &vec.x);
}

} // namespace renderer
