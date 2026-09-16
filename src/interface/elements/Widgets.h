/*
 * Widgets.h
 */

#pragma once

#include "imgui.h"

#include <glm/glm.hpp>

#include <string>

namespace BulletRender {

// fwd
namespace render { class Material; }

namespace interface {

// every row starts with caption of fixed width, so panels read like table:
//
//   Name        [Cube]
//   Visible     [x]
//   Position
//   x 0.00   y 0.00   z 0.00

// layout
constexpr float LABEL_COLUMN_BASE = 130.0f;                             // width at scale 1.0
extern float LABEL_COLUMN_WIDTH;                                        // caption column

ImVec4 tagColor(const glm::vec3& color);
void fieldLabel(const char* label);                                     // caption left, control fills rest of row

// read only
void statRow(const char* label, const char* format, ...) IM_FMTARGS(2); // same caption column as fields
void errorText(const std::string& message);                             // drawn only when not empty

// horizontal, control fits beside caption:
//
// Label       [ value ]
// Checkbox    [x]

bool checkboxField(const char* label, bool& value);
bool dragScalarField(const char* label, float& value, float min, float max, const char* format);
bool dragScalarField(const char* label, int& value, int min, int max, const char* format);
bool inputTextField(const char* label, char* buffer, size_t size, const char* hint = nullptr);
bool textField(const char* label, std::string& value, const char* hint = nullptr);   // same, through a std::string
bool comboField(const char* label, int& value, const char* const* options, int count);
bool bitsField(const char* label, unsigned& value, int count);      // grid of checkboxes, one per bit

// vertical, three components need whole row:
//
// Label
// x 0.00   y 0.00   z 0.00

bool dragVector3(const char* label, glm::vec3& value, float speed, float min, float max, const char* format);
bool checkboxAxes(const char* label, bool& x, bool& y, bool& z);        // three toggles, one per axis
bool dragColor3(const char* label, glm::vec3& color);                   // same plus swatch that opens picker

// same controls without caption, for rows that drew their own
bool dragScalarBare(float& value, float min, float max, const char* format);
bool dragColor3Bare(glm::vec3& color);

// menus

// right click menu, body draws the items
template <typename Body>
void contextMenu(const char* id, Body body)
{
    if (ImGui::BeginPopupContextItem(id))
    {
        body();
        ImGui::EndPopup();
    }
}

// same, opened by right click anywhere in the window background
template <typename Body>
void windowContextMenu(const char* id, Body body)
{
    if (ImGui::BeginPopupContextWindow(id, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        body();
        ImGui::EndPopup();
    }
}

// texture slots of a material, with previews and a load field
inline constexpr size_t TEXTURE_PATH_LENGTH = 256;

struct TextureFieldState {
    char path[TEXTURE_PATH_LENGTH] = "";
    std::string error;
};

void materialTextures(const char* id, render::Material& material, TextureFieldState& state);

// composite
void splitter(const char* id, float& fraction, float minFraction, float maxFraction);   // draggable gap between panes
bool loadFromFileField(const char* id, char* path, size_t size, const char* hint, const char* dragType = nullptr);   // path with load button, drop target when a type is given

// opt-in, toggle after caption greys out control until enabled:
//
// Color   [x]
// r 1.00   g 1.00   b 1.00

template <typename Widget>
bool overrideField(const char* label, bool& enabled, Widget widget)
{
    ImGui::PushID(label);

    fieldLabel(label);
    bool changed = ImGui::Checkbox("##override", &enabled);

    ImGui::BeginDisabled(!enabled);
    changed |= widget();
    ImGui::EndDisabled();

    ImGui::PopID();
    return changed;
}

} // namespace interface
} // namespace BulletRender
