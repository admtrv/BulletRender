/*
 * ObjLoader.h
 */

#pragma once

#include "Model.h"

#include <memory>
#include <string>

namespace BulletRender {
namespace scene {

// wavefront obj, geometry only, .mtl beside it is material importer's business
class ObjLoader {
public:
    static std::shared_ptr<Model> load(const std::string& path);
};

} // namespace scene
} // namespace BulletRender
