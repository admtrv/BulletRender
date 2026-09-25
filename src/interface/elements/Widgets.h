/*
 * Widgets.h
 */

#pragma once

#include "interface/elements/TreeView.h"

#include "imgui.h"

#include <glm/glm.hpp>

#include <string>
#include <unordered_set>

namespace BulletRender {
namespace interface {

// every row starts with caption of fixed width, so panels read like table:
//
//   Name        [Cube]
//   Visible     [x]
//   Position
//   x 0.00   y 0.00   z 0.00

// ---------------------------------------------------------------------------
// shared pieces
// ---------------------------------------------------------------------------

constexpr float LABEL_COLUMN_BASE = 130.0f;                             // width at scale 1.0
extern float LABEL_COLUMN_WIDTH;                                        // caption column

void fieldLabel(const char* label);                                     // caption left, control fills rest of row
ImVec4 tagColor(const glm::vec3& color);                                // lifted to read on dark ui

// ---------------------------------------------------------------------------
// read only
//
//   Triangles   1024
//   failed to load teapot.obj
// ---------------------------------------------------------------------------

void statRow(const char* label, const char* format, ...) IM_FMTARGS(2); // same caption column as fields
void errorText(const std::string& message);                             // drawn only when not empty

// ---------------------------------------------------------------------------
// horizontal, control fits beside caption
//
//   Name        [ Cube ]
//   Visible     [x]
// ---------------------------------------------------------------------------

bool checkboxField(const char* label, bool& value);
bool dragScalarField(const char* label, float& value, float min, float max, const char* format);
bool dragScalarField(const char* label, int& value, int min, int max, const char* format);
bool inputTextField(const char* label, char* buffer, size_t size, const char* hint = nullptr);
bool textField(const char* label, std::string& value, const char* hint = nullptr);   // same, through a std::string
bool comboField(const char* label, int& value, const char* const* options, int count);
bool bitsField(const char* label, unsigned& value, int count);          // grid of checkboxes, one per bit

// ---------------------------------------------------------------------------
// vertical, three components need whole row
//
//   Position
//   x 0.00   y 0.00   z 0.00
// ---------------------------------------------------------------------------

bool dragVector3(const char* label, glm::vec3& value, float speed, float min, float max, const char* format);
bool checkboxAxes(const char* label, bool& x, bool& y, bool& z);        // three toggles, one per axis
bool dragColor3(const char* label, glm::vec3& color);                   // same plus swatch that opens picker

// ---------------------------------------------------------------------------
// bare, for rows that drew their own caption
// ---------------------------------------------------------------------------

bool dragScalarBare(float& value, float min, float max, const char* format);
bool dragColor3Bare(glm::vec3& color);

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

// same, for the values an override usually carries
bool overrideField(const char* label, bool& enabled, glm::vec3& color);
bool overrideField(const char* label, bool& enabled, float& value, float min, float max, const char* format);

// ---------------------------------------------------------------------------
// file browser
//
//   [ /home/admtrv/Game           ]
//   |-- Assets
//   |   `-- Teapot.obj
//   `-- Project.project
//                    [Cancel] [Open]
// ---------------------------------------------------------------------------

struct BrowserState {
    const char* title = "Choose File";  // what modal is called, says what is being picked
    char root[512] = "";                // where tree starts, home when left empty
    std::string chosen;                 // what row selection points at
    std::unordered_set<std::string> opened;     // folders user opened, rest stay shut
    bool open = false;                  // modal is up, until something is picked or it is dropped
};

// opens on first call, returns true once it closes, picked empty when nothing was taken
bool fileBrowser(const char* id, BrowserState& state, std::string& picked);

// ---------------------------------------------------------------------------
// asset slots
//
//   Model       Teapot.obj      [...] [Delete]
//               failed to load ...
// ---------------------------------------------------------------------------

enum class AssetAction : uint8_t {
    None,
    Load,       // path holds what was picked or dropped
    Clear       // slot emptied
};

struct AssetFieldState {
    char path[256] = "";        // what browser or drop left behind
    std::string error;
    BrowserState browser;
};

AssetAction assetField(const char* label, const char* current, bool filled, AssetFieldState& state, const char* dragType = nullptr);

const char* fileName(const char* path);

// ---------------------------------------------------------------------------
// composite
// ---------------------------------------------------------------------------

void splitter(const char* id, float& fraction, float minFraction, float maxFraction);   // draggable gap between panes

// ---------------------------------------------------------------------------
// menus
// ---------------------------------------------------------------------------

// right click menu, body draws items
template <typename Body>
void contextMenu(const char* id, Body body)
{
    if (ImGui::BeginPopupContextItem(id))
    {
        body();
        ImGui::EndPopup();
    }
}

// same, opened by right click anywhere in window background
template <typename Body>
void windowContextMenu(const char* id, Body body)
{
    if (ImGui::BeginPopupContextWindow(id, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        body();
        ImGui::EndPopup();
    }
}

} // namespace interface
} // namespace BulletRender
