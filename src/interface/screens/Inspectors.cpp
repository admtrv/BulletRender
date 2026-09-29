/*
 * Inspectors.cpp
 */

#include "interface/Editor.h"

#include "interface/elements/Widgets.h"
#include "Colors.h"
#include "render/MaterialImport.h"
#include "render/textures/TextureLoader.h"
#include "scene/models/ModelLoader.h"

#include "imgui.h"

#include <algorithm>
#include <cstring>

namespace BulletRender {
namespace interface {

// transform
constexpr float DRAG_SPEED_POSITION = 0.05f;
constexpr float DRAG_SPEED_ROTATION = 0.5f;
constexpr float DRAG_SPEED_SCALE = 0.01f;
constexpr float SCALE_MINIMUM = 0.001f;

// light
constexpr float INTENSITY_MAXIMUM = 50.0f;
constexpr float RANGE_MAXIMUM = 500.0f;
constexpr float SHADOW_SIZE_MAXIMUM = 200.0f;
constexpr float CONE_ANGLE_MAXIMUM = 89.0f;

// camera
constexpr float FOV_MINIMUM = 10.0f;
constexpr float FOV_MAXIMUM = 120.0f;
constexpr float CLIP_MINIMUM = 0.01f;           // near at zero collapses depth precision
constexpr float CLIP_MAXIMUM = 5000.0f;
constexpr float FLY_SPEED_MAXIMUM = 50.0f;
constexpr float ORBIT_RADIUS_MINIMUM = 0.1f;
constexpr float ORBIT_RADIUS_MAXIMUM = 200.0f;

// material
constexpr float SHININESS_MINIMUM = 1.0f;
constexpr float SHININESS_MAXIMUM = 256.0f;

// uv, tiling below one stretches the picture, offset past one wraps back round
constexpr float UV_DRAG_SPEED = 0.01f;
constexpr float UV_SCALE_MINIMUM = 0.01f;
constexpr float UV_SCALE_MAXIMUM = 64.0f;
constexpr float UV_OFFSET_MINIMUM = -16.0f;
constexpr float UV_OFFSET_MAXIMUM = 16.0f;

// type names, indexed by matching enum
static const char* const LIGHT_TYPE_NAMES[] = {"Ambient", "Directional", "Point", "Spot"};
static const char* const CAMERA_TYPE_NAMES[] = {"Static", "Fly", "Orbit"};

static const char* lightTypeName(scene::LightType type)
{
    return LIGHT_TYPE_NAMES[static_cast<int>(type)];
}

static const char* cameraTypeName(scene::CameraType type)
{
    return CAMERA_TYPE_NAMES[static_cast<int>(type)];
}

// editable display name, every entity has one
static void nameField(scene::Named& named)
{
    std::string name = named.getName();

    if (textField("Name", name))
    {
        named.setName(name);
    }
}

// inspector sections

void Editor::drawInspector()
{
    ImGui::TextUnformatted("Inspector");

    if (!ImGui::BeginChild("Inspector", {0.0f, 0.0f}, ImGuiChildFlags_Borders))
    {
        ImGui::EndChild();
        return;
    }

    switch (m_selection.type)
    {
        case SelectionType::Object:
        {
            if (m_selection.index < m_scene.getObjects().size())
            {
                drawObjectInspector(*m_scene.getObjects()[m_selection.index]);
            }
            else
            {
                m_selection.clear();
            }
            break;
        }
        case SelectionType::Light:
        {
            if (m_selection.index < m_scene.getLights().size())
            {
                drawLightInspector(*m_scene.getLights()[m_selection.index]);
            }
            else
            {
                m_selection.clear();
            }
            break;
        }
        case SelectionType::Camera:
        {
            if (m_selection.index < m_scene.getCameras().size())
            {
                drawCameraInspector(*m_scene.getCameras()[m_selection.index], m_selection.index);
            }
            else
            {
                m_selection.clear();
            }
            break;
        }
        case SelectionType::None:
        {
            ImGui::TextDisabled("Nothing selected");
            break;
        }
    }

    ImGui::EndChild();
}

void Editor::drawObjectInspector(scene::SceneObject& object)
{
    nameField(object);

    bool visible = object.isVisible();
    if (checkboxField("Visible", visible))
    {
        object.setVisible(visible);
    }

    drawTransformInspector(object.getTransform());
    drawModelInspector(object);
    drawMaterialInspector(object.getMaterial());

    ImGui::Separator();
    if (ImGui::Button("Delete"))
    {
        m_scene.removeObject(m_selection.index);
        m_selection.clear();
    }
}

void Editor::drawLightInspector(scene::Light& light)
{
    nameField(light);

    statRow("Type", "%s", lightTypeName(light.getType()));

    bool visible = light.isVisible();
    if (checkboxField("Visible", visible))
    {
        light.setVisible(visible);
    }

    glm::vec3 color = light.getColor();
    if (dragColor3("Color", color))
    {
        light.setColor(color);
    }

    float intensity = light.getIntensity();
    if (dragScalarField("Intensity", intensity, 0.0f, INTENSITY_MAXIMUM, "%.2f"))
    {
        light.setIntensity(intensity);
    }

    // only these two reach shadow pass, flag does nothing elsewhere
    const scene::LightType type = light.getType();
    if (type == scene::LightType::Directional || type == scene::LightType::Spot)
    {
        bool shadow = light.getCastsShadow();
        if (checkboxField("Casts shadow", shadow))
        {
            light.setCastsShadow(shadow);
        }
    }

    // type specific parameters
    switch (type)
    {
        case scene::LightType::Directional:
        {
            auto& directional = static_cast<scene::DirectionalLight&>(light);

            glm::vec3 direction = directional.getDirection();
            if (dragVector3("Direction", direction, 0.01f, -1.0f, 1.0f, "%.2f"))
            {
                directional.setDirection(direction);
            }

            glm::vec3 target = directional.getShadowTarget();
            if (dragVector3("Shadow target", target, DRAG_SPEED_POSITION, 0.0f, 0.0f, "%.2f"))
            {
                directional.setShadowTarget(target);
            }

            float orthoSize = directional.getShadowOrthoSize();
            if (dragScalarField("Shadow size", orthoSize, 1.0f, SHADOW_SIZE_MAXIMUM, "%.1f"))
            {
                directional.setShadowOrthoSize(orthoSize);
            }
            break;
        }
        case scene::LightType::Point:
        {
            auto& point = static_cast<scene::PointLight&>(light);

            glm::vec3 position = point.getPosition();
            if (dragVector3("Position", position, DRAG_SPEED_POSITION, 0.0f, 0.0f, "%.2f"))
            {
                point.setPosition(position);
            }

            float range = point.getRange();
            if (dragScalarField("Range", range, 0.0f, RANGE_MAXIMUM, "%.1f"))
            {
                point.setRange(range);
            }
            break;
        }
        case scene::LightType::Spot:
        {
            auto& spot = static_cast<scene::SpotLight&>(light);

            glm::vec3 position = spot.getPosition();
            if (dragVector3("Position", position, DRAG_SPEED_POSITION, 0.0f, 0.0f, "%.2f"))
            {
                spot.setPosition(position);
            }

            glm::vec3 direction = spot.getDirection();
            if (dragVector3("Direction", direction, 0.01f, -1.0f, 1.0f, "%.2f"))
            {
                spot.setDirection(direction);
            }

            // cones stored as cosines, edited as degrees
            float inner = glm::degrees(std::acos(spot.getInnerCos()));
            float outer = glm::degrees(std::acos(spot.getOuterCos()));
            // both rows draw every frame, short circuit makes one flicker
            bool conesChanged = dragScalarField("Inner angle", inner, 0.0f, outer, "%.1f");
            conesChanged |= dragScalarField("Outer angle", outer, inner, CONE_ANGLE_MAXIMUM, "%.1f");

            if (conesChanged)
            {
                spot.setCones(inner, outer);
            }

            float range = spot.getRange();
            if (dragScalarField("Range", range, 0.0f, RANGE_MAXIMUM, "%.1f"))
            {
                spot.setRange(range);
            }
            break;
        }
        case scene::LightType::Ambient:
        {
            break;
        }
    }

    ImGui::Separator();
    if (ImGui::Button("Delete"))
    {
        m_scene.removeLight(m_selection.index);
        m_selection.clear();
    }
}

void Editor::drawCameraInspector(scene::Camera& camera, size_t index)
{
    nameField(camera);

    statRow("Type", "%s", cameraTypeName(camera.getType()));

    const bool active = &camera == m_scene.getActiveCamera();
    statRow("Status", "%s", active ? "Active" : "Inactive");

    if (!active && ImGui::Button("Make active"))
    {
        m_scene.setActiveCamera(&camera);
    }

    ImGui::Separator();

    // controlled camera rewrites own pose every frame, editing fights input
    const bool driven = active && camera.getType() != scene::CameraType::Static;
    ImGui::BeginDisabled(driven);

    glm::vec3 position = camera.getPosition();
    if (dragVector3("Position", position, DRAG_SPEED_POSITION, 0.0f, 0.0f, "%.2f"))
    {
        camera.setPosition(position);
    }

    ImGui::EndDisabled();

    float fov = camera.getFov();
    if (dragScalarField("Field of view", fov, FOV_MINIMUM, FOV_MAXIMUM, "%.0f"))
    {
        camera.setFov(fov);
    }

    // near stays in front of eye and behind far, else projection degenerates
    float zNear = camera.getNear();
    float zFar = camera.getFar();

    bool clipChanged = dragScalarField("Near", zNear, CLIP_MINIMUM, zFar - CLIP_MINIMUM, "%.2f");
    clipChanged |= dragScalarField("Far", zFar, zNear + CLIP_MINIMUM, CLIP_MAXIMUM, "%.1f");

    if (clipChanged)
    {
        camera.setClipPlanes(zNear, zFar);
    }

    // type specific parameters
    switch (camera.getType())
    {
        case scene::CameraType::Static:
        {
            auto& staticCamera = static_cast<scene::StaticCamera&>(camera);

            glm::vec3 target = staticCamera.getTarget();
            if (dragVector3("Target", target, DRAG_SPEED_POSITION, 0.0f, 0.0f, "%.2f"))
            {
                staticCamera.setTarget(target);
            }
            break;
        }
        case scene::CameraType::Fly:
        {
            auto& flyCamera = static_cast<scene::FlyCamera&>(camera);

            float speed = flyCamera.getSpeed();
            if (dragScalarField("Speed", speed, 0.1f, FLY_SPEED_MAXIMUM, "%.1f"))
            {
                flyCamera.setSpeed(speed);
            }
            break;
        }
        case scene::CameraType::Orbit:
        {
            auto& orbitCamera = static_cast<scene::OrbitCamera&>(camera);

            glm::vec3 target = orbitCamera.getTarget();
            if (dragVector3("Target", target, DRAG_SPEED_POSITION, 0.0f, 0.0f, "%.2f"))
            {
                orbitCamera.setTarget(target);
            }

            float radius = orbitCamera.getRadius();
            if (dragScalarField("Radius", radius, ORBIT_RADIUS_MINIMUM, ORBIT_RADIUS_MAXIMUM, "%.1f"))
            {
                orbitCamera.setRadius(radius);
            }
            break;
        }
    }

    ImGui::Separator();
    if (ImGui::Button("Delete"))
    {
        m_scene.removeCamera(index);
        m_selection.clear();
    }
}

void Editor::drawTransformInspector(scene::Transform& transform)
{
    if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    glm::vec3 position = transform.getLocalPosition();
    if (dragVector3("Position", position, DRAG_SPEED_POSITION, 0.0f, 0.0f, "%.2f"))
    {
        transform.setLocalPosition(position);
    }

    // quaternions cannot be edited by hand, euler angles are editable form
    glm::vec3 euler = glm::degrees(glm::eulerAngles(transform.getLocalRotation()));
    if (dragVector3("Rotation", euler, DRAG_SPEED_ROTATION, 0.0f, 0.0f, "%.1f"))
    {
        transform.setLocalRotation(glm::quat(glm::radians(euler)));
    }

    glm::vec3 scale = transform.getLocalScale();
    if (dragVector3("Scale", scale, DRAG_SPEED_SCALE, SCALE_MINIMUM, 100.0f, "%.2f"))
    {
        transform.setLocalScale(glm::max(scale, glm::vec3(SCALE_MINIMUM)));
    }

    if (ImGui::Button("Reset"))
    {
        transform.reset();
    }
}

void Editor::drawModelInspector(scene::SceneObject& object)
{
    if (!ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    if (const std::shared_ptr<scene::Model>& model = object.getModel())
    {
        const glm::vec3 size = model->getBoundsMax() - model->getBoundsMin();


        statRow("Vertices", "%u", model->getVertexCount());
        statRow("Triangles", "%u", model->getTriangleCount());
        statRow("Submeshes", "%zu", model->getMeshes().size());     // one draw call each
        statRow("Size", "%.2f  %.2f  %.2f", size.x, size.y, size.z);

        ImGui::Separator();
    }
    else
    {
        ImGui::TextDisabled("No model");
    }

    const bool filled = object.getModel() != nullptr;

    switch (assetField("Model", filled ? fileName(m_modelField.path) : "None", filled, m_modelField))
    {
        case AssetAction::Clear:
            object.setModel(nullptr);
            m_modelField.error.clear();
            break;

        case AssetAction::Load:
            // loaded geometry joins scene and goes to selected object
            if (std::shared_ptr<scene::Model> model = scene::ModelLoader::instance().load(m_modelField.path))
            {
                object.setModel(std::move(model));
                m_modelField.error.clear();
            }
            else
            {
                m_modelField.error = "failed to load " + std::string(m_modelField.path);
            }
            break;

        default:
            break;
    }
}

// what each slot is called, in the order the inspector lays them out
static const char* const SLOT_LABELS[MATERIAL_SLOT_COUNT] = {"Diffuse", "Specular", "Normal", "Emissive"};

static const char* const SHADING_NAMES[] = {"Lit", "Unlit"};
static const char* const ALPHA_NAMES[] = {"Opaque", "Mask", "Blend"};
static const char* const FILTER_NAMES[] = {"Smooth", "Pixel"};
static const char* const WRAP_NAMES[] = {"Repeat", "Clamp", "Mirror"};

void Editor::drawMaterialInspector(render::Material& material)
{
    if (!ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    // naming a file fills the fields below once, nothing reads it again afterwards
    switch (assetField("Source", m_sourceField.path[0] ? fileName(m_sourceField.path) : "None", m_sourceField.path[0] != '\0', m_sourceField))
    {
        case AssetAction::Clear:
            m_sourceField.path[0] = '\0';
            break;

        case AssetAction::Load:
            importMaterial(material, m_sourceField.path);
            break;

        default:
            break;
    }

    int shading = int(material.shading);

    if (comboField("Shading", shading, SHADING_NAMES, IM_ARRAYSIZE(SHADING_NAMES)))
    {
        material.shading = render::Shading(shading);
    }

    const bool lit = material.shading == render::Shading::Lit;

    dragColor3("Diffuse", material.diffuse);
    drawSlot(SLOT_LABELS[0], material.diffuseTexture, m_slotFields[0]);

    // light shapes nothing on a flat picture, these terms would sit dead
    if (lit)
    {
        dragColor3("Specular", material.specular);
        drawSlot(SLOT_LABELS[1], material.specularTexture, m_slotFields[1]);

        // tight highlight only shows at right angle, low values make specular obvious
        dragScalarField("Shininess", material.shininess, SHININESS_MINIMUM, SHININESS_MAXIMUM, "%.0f");

        drawSlot(SLOT_LABELS[2], material.normalTexture, m_slotFields[2]);
    }

    dragColor3("Emissive", material.emissive);
    drawSlot(SLOT_LABELS[3], material.emissiveTexture, m_slotFields[3]);

    int alpha = int(material.alphaMode);

    if (comboField("Alpha Mode", alpha, ALPHA_NAMES, IM_ARRAYSIZE(ALPHA_NAMES)))
    {
        material.alphaMode = render::AlphaMode(alpha);
    }

    if (material.alphaMode == render::AlphaMode::Mask)
    {
        dragScalarField("Alpha Cutoff", material.alphaCutoff, 0.0f, 1.0f, "%.2f");
    }

    checkboxField("Double Sided", material.doubleSided);
}

// only what the file names is taken, an empty entry leaves its slot as it stands
void Editor::importMaterial(render::Material& material, const std::string& path)
{
    const std::vector<render::MaterialImport> imported = render::readMtl(path);

    if (imported.empty())
    {
        return;
    }

    const render::MaterialImport& source = imported.front();

    material.diffuse = source.diffuse;
    material.specular = source.specular;
    material.emissive = source.emissive;
    material.shininess = source.shininess;

    const std::pair<render::TextureSlot*, const std::string*> slots[MATERIAL_SLOT_COUNT] = {
        {&material.diffuseTexture, &source.diffuseTexture},
        {&material.specularTexture, &source.specularTexture},
        {&material.normalTexture, &source.normalTexture},
        {&material.emissiveTexture, &source.emissiveTexture}
    };

    for (int slot = 0; slot < MATERIAL_SLOT_COUNT; slot++)
    {
        auto& [target, texturePath] = slots[slot];

        target->texture = texturePath->empty() ? nullptr : render::TextureLoader::instance().load(*texturePath);

        std::snprintf(m_slotFields[slot].path, sizeof(m_slotFields[slot].path), "%s", texturePath->c_str());
    }
}

// one picture with how it is read and what part of it is taken
void Editor::drawSlot(const char* label, render::TextureSlot& slot, AssetFieldState& state)
{
    ImGui::PushID(label);

    switch (assetField(label, slot.empty() ? "None" : fileName(state.path), !slot.empty(), state))
    {
        case AssetAction::Clear:
            slot.texture.reset();
            state.error.clear();
            break;

        case AssetAction::Load:
            slot.texture = render::TextureLoader::instance().load(state.path);
            state.error = slot.texture ? std::string{} : "failed to load " + std::string(state.path);
            break;

        default:
            break;
    }

    // settings belong to what was taken, an empty slot has nothing to read
    if (!slot.empty())
    {
        ImGui::Indent();

        int filter = int(slot.sampler.filter);

        if (comboField("Filter", filter, FILTER_NAMES, IM_ARRAYSIZE(FILTER_NAMES)))
        {
            slot.sampler.filter = render::TextureFilter(filter);
        }

        int wrapU = int(slot.sampler.wrapU);
        int wrapV = int(slot.sampler.wrapV);

        if (comboField("Wrap U", wrapU, WRAP_NAMES, IM_ARRAYSIZE(WRAP_NAMES)))
        {
            slot.sampler.wrapU = render::TextureWrap(wrapU);
        }

        if (comboField("Wrap V", wrapV, WRAP_NAMES, IM_ARRAYSIZE(WRAP_NAMES)))
        {
            slot.sampler.wrapV = render::TextureWrap(wrapV);
        }

        dragVector2("Tiling", slot.uvScale, UV_DRAG_SPEED, UV_SCALE_MINIMUM, UV_SCALE_MAXIMUM, "%.2f");
        dragVector2("Offset", slot.uvOffset, UV_DRAG_SPEED, UV_OFFSET_MINIMUM, UV_OFFSET_MAXIMUM, "%.2f");

        ImGui::Unindent();
    }

    errorText(state.error);

    ImGui::PopID();
}

} // namespace interface
} // namespace BulletRender
