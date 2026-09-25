/*
 * Settings.cpp
 */

#include "interface/Editor.h"

#include "interface/elements/Fonts.h"
#include "interface/elements/Widgets.h"
#include "Config.h"
#include "app/Loop.h"
#include "app/Window.h"
#include "render/Renderer.h"

#include <array>

#include "imgui.h"

namespace BulletRender {
namespace interface {

// frame
constexpr int FRAME_LIMIT_MINIMUM = 15;         // below this panel becomes hard to use
constexpr int FRAME_LIMIT_MAXIMUM = 240;
constexpr int FRAME_LIMIT_DEFAULT = 120;        // what turning limit back on falls to

// interface
constexpr float UI_SCALE_MINIMUM = 0.8f;
constexpr float UI_SCALE_MAXIMUM = 2.0f;

// environment
constexpr const char* BACKGROUND_OPTIONS[] = {"Color", "Skybox"};
constexpr const char* LAYOUT_OPTIONS[] = {"Cross", "Faces"};
constexpr const char* FACE_LABELS[6] = {"Right", "Left", "Top", "Bottom", "Front", "Back"};

void Editor::drawSettings(float dt)
{
    drawFrameSection(dt);
    drawInterfaceSection();
    drawEnvironmentSection();
    drawDebugSection();
}

void Editor::drawFrameSection(float dt)
{
    if (!ImGui::CollapsingHeader("Frame", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    statRow("FPS", "%.1f", m_fps);
    statRow("Frame time", "%.2f ms", dt * 1000.0f);
    statRow("Aspect", "%.3f", render::Renderer::getAspect());

    bool vsync = app::Window::isVSync();
    if (checkboxField("VSync", vsync))
    {
        app::Window::setVSync(vsync);
    }

    // vsync already paces frames, limiter would fight it
    ImGui::BeginDisabled(vsync);

    // unlimited is own toggle, dragging through single digit rates
    // would freeze ui with no way back
    int limit = app::Loop::getFrameRateLimit();

    bool unlimited = limit <= 0;
    if (checkboxField("Unlimited", unlimited))
    {
        app::Loop::setFrameRateLimit(unlimited ? 0 : FRAME_LIMIT_DEFAULT);
        limit = app::Loop::getFrameRateLimit();
    }

    ImGui::BeginDisabled(unlimited);

    int shown = unlimited ? FRAME_LIMIT_DEFAULT : limit;
    if (dragScalarField("Max FPS", shown, FRAME_LIMIT_MINIMUM, FRAME_LIMIT_MAXIMUM, "%d") && !unlimited)
    {
        app::Loop::setFrameRateLimit(shown);
    }

    ImGui::EndDisabled();
    ImGui::EndDisabled();
}

void Editor::drawInterfaceSection()
{
    if (!ImGui::CollapsingHeader("Interface", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    if (dragScalarField("Font size", m_uiScale, UI_SCALE_MINIMUM, UI_SCALE_MAXIMUM, "%.2f"))
    {
        Fonts::setScale(m_uiScale);

        // captions grow with font, column follows or they overlap control
        LABEL_COLUMN_WIDTH = LABEL_COLUMN_BASE * m_uiScale;
    }
}

void Editor::drawEnvironmentSection()
{
    if (!ImGui::CollapsingHeader("Environment", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    int background = int(m_background);

    if (comboField("Background", background, BACKGROUND_OPTIONS, IM_ARRAYSIZE(BACKGROUND_OPTIONS)))
    {
        m_background = Background(background);
        applyBackground();
    }

    if (m_background == Background::Color)
    {
        const glm::vec4& current = render::Renderer::getBackgroundColor();

        glm::vec3 color = glm::vec3(current);

        if (dragColor3("Color", color))
        {
            render::Renderer::setBackgroundColor(glm::vec4(color, current.a));
        }

        return;
    }

    int layout = int(m_skyLayout);

    if (comboField("Layout", layout, LAYOUT_OPTIONS, IM_ARRAYSIZE(LAYOUT_OPTIONS)))
    {
        m_skyLayout = SkyLayout(layout);
        applyBackground();
    }

    // slot taken or emptied, either way sky is built anew from what is left
    const auto slotChanged = [this](const char* label, AssetFieldState& slot) {
        const bool filled = slot.path[0] != '\0';

        switch (assetField(label, filled ? fileName(slot.path) : "None", filled, slot))
        {
            case AssetAction::Clear:
                slot.path[0] = '\0';
                applyBackground();
                break;

            case AssetAction::Load:
                applyBackground();
                break;

            default:
                break;
        }
    };

    if (m_skyLayout == SkyLayout::Cross)
    {
        slotChanged("Texture", m_crossField);
        return;
    }

    for (int face = 0; face < 6; face++)
    {
        slotChanged(FACE_LABELS[face], m_faceFields[face]);
    }
}

void Editor::applyBackground()
{
    if (!m_skybox)
    {
        return;
    }

    if (m_background != Background::Skybox)
    {
        m_skybox->setEnabled(false);
        return;
    }

    if (m_skyLayout == SkyLayout::Cross && m_crossField.path[0] != '\0')
    {
        m_skybox->setCubeMap(std::make_shared<render::CubeMap>(std::string(m_crossField.path)));
        m_skybox->setEnabled(true);

        return;
    }

    if (m_skyLayout == SkyLayout::Faces)
    {
        std::array<std::string, 6> faces;

        for (int face = 0; face < 6; face++)
        {
            if (m_faceFields[face].path[0] == '\0')
            {
                m_skybox->setEnabled(false);
                return;
            }

            faces[face] = m_faceFields[face].path;
        }

        m_skybox->setCubeMap(std::make_shared<render::CubeMap>(faces));
        m_skybox->setEnabled(true);

        return;
    }

    m_skybox->setEnabled(false);
}

void Editor::drawDebugSection()
{
    if (!ImGui::CollapsingHeader("Debug draw", ImGuiTreeNodeFlags_DefaultOpen))
    {
        return;
    }

    checkboxField("Show gizmos", m_showDebug);

    // individual gizmos mean nothing while overlay is off
    ImGui::BeginDisabled(!m_showDebug);

    bool transforms = m_debug.getShowTransforms();
    if (checkboxField("Transforms", transforms))
    {
        m_debug.setShowTransforms(transforms);
    }

    bool lights = m_debug.getShowLights();
    if (checkboxField("Lights", lights))
    {
        m_debug.setShowLights(lights);
    }

    bool bounds = m_debug.getShowBounds();
    if (checkboxField("Bounds", bounds))
    {
        m_debug.setShowBounds(bounds);
    }

    bool cameras = m_debug.getShowCameras();
    if (checkboxField("Cameras", cameras))
    {
        m_debug.setShowCameras(cameras);
    }

    ImGui::EndDisabled();
}

} // namespace interface
} // namespace BulletRender
