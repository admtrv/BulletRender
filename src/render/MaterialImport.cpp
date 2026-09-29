/*
 * MaterialImport.cpp
 */

#include "MaterialImport.h"

#include "tiny_obj_loader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

namespace BulletRender {
namespace render {

// .mtl names its pictures relative to itself
static std::string beside(const std::filesystem::path& directory, const std::string& name)
{
    return name.empty() ? std::string{} : (directory / name).string();
}

std::vector<MaterialImport> readMtl(const std::string& path)
{
    std::ifstream file(path);

    // model need not come with one, then there is simply nothing to take
    if (!file)
    {
        return {};
    }

    std::map<std::string, int> index;
    std::vector<tinyobj::material_t> parsed;

    std::string warning;
    std::string error;

    tinyobj::LoadMtl(&index, &parsed, &file, &warning, &error);

    if (!error.empty())
    {
        std::cerr << "mtl error: " << error << '\n';
        return {};
    }

    const std::filesystem::path directory = std::filesystem::path(path).parent_path();

    std::vector<MaterialImport> materials;
    materials.reserve(parsed.size());

    for (const tinyobj::material_t& entry : parsed)
    {
        MaterialImport material;

        material.name = entry.name;

        material.diffuse = {entry.diffuse[0], entry.diffuse[1], entry.diffuse[2]};
        material.specular = {entry.specular[0], entry.specular[1], entry.specular[2]};
        material.emissive = {entry.emission[0], entry.emission[1], entry.emission[2]};
        material.shininess = entry.shininess;

        material.diffuseTexture = beside(directory, entry.diffuse_texname);
        material.specularTexture = beside(directory, entry.specular_texname);
        material.emissiveTexture = beside(directory, entry.emissive_texname);

        // bump and norm both stand for the same slot, whichever the exporter wrote
        material.normalTexture = beside(directory, entry.normal_texname.empty() ? entry.bump_texname : entry.normal_texname);

        materials.push_back(std::move(material));
    }

    return materials;
}

} // namespace render
} // namespace BulletRender
