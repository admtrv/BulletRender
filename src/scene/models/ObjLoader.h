/*
 * ObjLoader.h
 */

#pragma once

#include "ModelLoader.h"

#include <string>
#include <vector>

namespace BulletRender {
namespace scene {

// wavefront obj, geometry only, .mtl beside it is material importer's business
class ObjLoader {
public:
    static std::vector<MeshData> read(const std::string& path);
};

} // namespace scene
} // namespace BulletRender
