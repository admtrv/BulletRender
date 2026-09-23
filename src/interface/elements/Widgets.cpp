/*
 * Widgets.cpp
 */

#include "interface/elements/Widgets.h"

#include "Colors.h"
#include "Config.h"

#include "imgui_stdlib.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include <vector>

namespace BulletRender {
namespace interface {

constexpr float TAG_BRIGHTNESS = 1.6f;          // palette tuned for 3d view, tags need more punch on dark ui
constexpr float COLOR_DRAG_SPEED = 0.005f;      // channels live in 0..1, steps must be small
constexpr float BROWSER_WIDTH = 520.0f;
constexpr float BROWSER_LIST_HEIGHT = 320.0f;
constexpr float MIN_SLOT_WIDTH = 24.0f;         // narrow panel still leaves something to drop onto
constexpr int BROWSER_MAX_DEPTH = 32;           // deep enough for any tree, short of loop

constexpr float SPLITTER_THICKNESS = 6.0f;      // hit area, drawn line is thinner
constexpr float DRAG_RANGE_FRACTION = 0.002f;   // drag speed as share of range, keeps fields feeling alike

constexpr int BITS_PER_ROW = 8;                 // mask splits into bytes, rows stay readable
constexpr int AXIS_COUNT = 3;

// wide enough for longest caption, scaled with ui font
float LABEL_COLUMN_WIDTH = LABEL_COLUMN_BASE * config::FontScale;

// ---------------------------------------------------------------------------
// shared pieces
// ---------------------------------------------------------------------------

// what three tagged cells are named and painted with
static const char* const AXIS_NAMES[AXIS_COUNT] = {"x", "y", "z"};
static const char* const CHANNEL_NAMES[AXIS_COUNT] = {"r", "g", "b"};
static const glm::vec3 AXIS_COLORS[AXIS_COUNT] = {colors::AxisX, colors::AxisY, colors::AxisZ};
static const glm::vec3 CHANNEL_COLORS[AXIS_COUNT] = {colors::Red, colors::Green, colors::Blue};

// three tagged cells side by side, control draws what sits in each
//
// x 0.00   y 0.00   z 0.00
template <class Cell>
static bool taggedRow(const char* const names[AXIS_COUNT], const glm::vec3 tints[AXIS_COUNT], float width, Cell cell)
{
    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    const float cellWidth = (width - spacing * 2.0f) / AXIS_COUNT;

    // widest tag sets offset for all three, otherwise narrower glyph
    // lets its field start earlier and columns drift apart
    float tagWidth = 0.0f;
    for (int component = 0; component < AXIS_COUNT; component++)
    {
        tagWidth = std::max(tagWidth, ImGui::CalcTextSize(names[component]).x);
    }
    tagWidth += spacing;

    const float startX = ImGui::GetCursorPosX();

    bool changed = false;

    for (int component = 0; component < AXIS_COUNT; component++)
    {
        ImGui::PushID(component);

        // cells placed absolutely, rounding in one does not shift next
        const float cellX = startX + static_cast<float>(component) * (cellWidth + spacing);

        ImGui::SetCursorPosX(cellX);
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(tagColor(tints[component]), "%s", names[component]);

        ImGui::SameLine();
        ImGui::SetCursorPosX(cellX + tagWidth);

        changed |= cell(component, cellWidth - tagWidth);

        ImGui::PopID();

        if (component < AXIS_COUNT - 1)
        {
            ImGui::SameLine();
        }
    }

    return changed;
}

// same row filled with drags, what vectors and colors both come down to
static bool dragComponents(glm::vec3& value, const char* const names[AXIS_COUNT], const glm::vec3 tints[AXIS_COUNT],
                           float speed, float min, float max, const char* format, float width)
{
    return taggedRow(names, tints, width, [&](int component, float cellWidth) {
        ImGui::SetNextItemWidth(cellWidth);
        return ImGui::DragFloat("##value", &value[component], speed, min, max, format);
    });
}

// channels plus swatch that opens picker, what both color widgets draw
static bool colorComponents(glm::vec3& color)
{
    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    const float swatchWidth = ImGui::GetFrameHeight();

    // swatch sits at end of row, fields give up its width up front
    bool changed = dragComponents(color, CHANNEL_NAMES, CHANNEL_COLORS, COLOR_DRAG_SPEED, 0.0f, 1.0f, "%.2f",
                                  ImGui::GetContentRegionAvail().x - swatchWidth - spacing);

    ImGui::SameLine(0.0f, spacing);
    changed |= ImGui::ColorEdit3("##swatch", &color.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

    return changed;
}

void fieldLabel(const char* label)
{
    // column starts where row does, reading cursor after text drifts per label
    const float startX = ImGui::GetCursorPosX();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    ImGui::SameLine(startX + LABEL_COLUMN_WIDTH);
}

ImVec4 tagColor(const glm::vec3& color)
{
    const glm::vec3 lifted = glm::min(color * TAG_BRIGHTNESS, glm::vec3(1.0f));
    return {lifted.r, lifted.g, lifted.b, 1.0f};
}

// caption, then control filling rest of row, what every horizontal field comes down to
template <class Control>
static bool labeledField(const char* label, Control control)
{
    ImGui::PushID(label);
    fieldLabel(label);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    const bool changed = control();

    ImGui::PopID();
    return changed;
}

// ---------------------------------------------------------------------------
// read only
//
//   Triangles   1024
//   failed to load teapot.obj
// ---------------------------------------------------------------------------

void statRow(const char* label, const char* format, ...)
{
    fieldLabel(label);

    va_list args;
    va_start(args, format);
    ImGui::TextV(format, args);
    va_end(args);
}

void errorText(const std::string& message)
{
    if (message.empty())
    {
        return;
    }

    ImGui::TextColored(tagColor(colors::Red), "%s", message.c_str());
}

// ---------------------------------------------------------------------------
// horizontal, control fits beside caption
//
//   Name        [ Cube ]
//   Visible     [x]
// ---------------------------------------------------------------------------

bool checkboxField(const char* label, bool& value)
{
    // checkbox is square, so it takes none of width row offers
    return labeledField(label, [&] { return ImGui::Checkbox("##value", &value); });
}

bool dragScalarField(const char* label, float& value, float min, float max, const char* format)
{
    const bool changed = labeledField(label, [&] {
        return ImGui::DragFloat("##value", &value, (max - min) * DRAG_RANGE_FRACTION, min, max, format);
    });

    value = glm::clamp(value, min, max);
    return changed;
}

bool dragScalarField(const char* label, int& value, int min, int max, const char* format)
{
    const bool changed = labeledField(label, [&] {
        return ImGui::DragInt("##value", &value, static_cast<float>(max - min) * DRAG_RANGE_FRACTION, min, max, format);
    });

    value = glm::clamp(value, min, max);
    return changed;
}

bool inputTextField(const char* label, char* buffer, size_t size, const char* hint)
{
    return labeledField(label, [&] {
        return hint != nullptr ? ImGui::InputTextWithHint("##value", hint, buffer, size) : ImGui::InputText("##value", buffer, size);
    });
}

bool textField(const char* label, std::string& value, const char* hint)
{
    return labeledField(label, [&] {
        return hint != nullptr ? ImGui::InputTextWithHint("##value", hint, &value) : ImGui::InputText("##value", &value);
    });
}

bool comboField(const char* label, int& value, const char* const* options, int count)
{
    return labeledField(label, [&] {
        const char* current = (value >= 0 && value < count) ? options[value] : "";

        if (!ImGui::BeginCombo("##value", current))
        {
            return false;
        }

        bool changed = false;

        for (int i = 0; i < count; i++)
        {
            if (ImGui::Selectable(options[i], i == value))
            {
                value = i;
                changed = true;
            }
        }

        ImGui::EndCombo();
        return changed;
    });
}

bool bitsField(const char* label, unsigned& value, int count)
{
    ImGui::PushID(label);

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);

    bool changed = false;

    // rows of eight, bit zero top left
    for (int bit = 0; bit < count; bit++)
    {
        if (bit % BITS_PER_ROW != 0)
        {
            ImGui::SameLine();
        }

        ImGui::PushID(bit);

        const unsigned flag = 1u << bit;
        bool set = (value & flag) != 0;

        if (ImGui::Checkbox("##bit", &set))
        {
            value = set ? (value | flag) : (value & ~flag);
            changed = true;
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("%d", bit);
        }

        ImGui::PopID();
    }

    ImGui::PopID();
    return changed;
}

// ---------------------------------------------------------------------------
// vertical, three components need whole row
//
//   Position
//   x 0.00   y 0.00   z 0.00
// ---------------------------------------------------------------------------

bool dragVector3(const char* label, glm::vec3& value, float speed, float min, float max, const char* format)
{
    ImGui::PushID(label);
    ImGui::TextUnformatted(label);

    const bool changed = dragComponents(value, AXIS_NAMES, AXIS_COLORS, speed, min, max, format,
                                        ImGui::GetContentRegionAvail().x);

    ImGui::PopID();
    return changed;
}

bool checkboxAxes(const char* label, bool& x, bool& y, bool& z)
{
    bool* const axes[AXIS_COUNT] = {&x, &y, &z};

    ImGui::PushID(label);
    ImGui::TextUnformatted(label);

    // same cells vector rows use, so toggles line up under them
    const bool changed = taggedRow(AXIS_NAMES, AXIS_COLORS, ImGui::GetContentRegionAvail().x, [&](int axis, float) {
        return ImGui::Checkbox("##axis", axes[axis]);
    });

    ImGui::PopID();
    return changed;
}

bool dragColor3(const char* label, glm::vec3& color)
{
    ImGui::PushID(label);
    ImGui::TextUnformatted(label);

    const bool changed = colorComponents(color);

    ImGui::PopID();
    return changed;
}

// ---------------------------------------------------------------------------
// bare, for rows that drew their own caption
// ---------------------------------------------------------------------------

bool dragScalarBare(float& value, float min, float max, const char* format)
{
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    const bool changed = ImGui::DragFloat("##value", &value, (max - min) * DRAG_RANGE_FRACTION, min, max, format);

    value = glm::clamp(value, min, max);
    return changed;
}

bool dragColor3Bare(glm::vec3& color)
{
    return colorComponents(color);
}

bool overrideField(const char* label, bool& enabled, glm::vec3& color)
{
    return overrideField(label, enabled, [&color]() { return dragColor3Bare(color); });
}

bool overrideField(const char* label, bool& enabled, float& value, float min, float max, const char* format)
{
    return overrideField(label, enabled, [&]() { return dragScalarBare(value, min, max, format); });
}

// ---------------------------------------------------------------------------
// file browser
//
//   [ /home/admtrv/Game           ]
//   |-- Assets
//   |   `-- Teapot.obj
//   `-- Project.project
//                    [Cancel] [Open]
// ---------------------------------------------------------------------------

namespace fs = std::filesystem;

bool fileBrowser(const char* id, BrowserState& state, std::string& picked);

// folders first, then names, as file manager reads
static std::vector<fs::directory_entry> listFolder(const fs::path& folder)
{
    std::vector<fs::directory_entry> entries;

    std::error_code error;
    for (const fs::directory_entry& entry : fs::directory_iterator(folder, error))
    {
        entries.push_back(entry);
    }

    std::sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
        const bool leftFolder = a.is_directory();

        return leftFolder == b.is_directory() ? a.path().filename() < b.path().filename() : leftFolder;
    });

    return entries;
}

// one row and whatever hangs under it, folders open in place
static void browserRow(TreeView& tree, BrowserState& state, const fs::directory_entry& entry, bool last, int depth)
{
    if (depth > BROWSER_MAX_DEPTH)
    {
        return;
    }

    const std::string path = entry.path().string();
    const bool directory = entry.is_directory();
    // shut until asked, walking whole filesystem would cost frame
    const bool open = state.opened.count(path) != 0;

    if (tree.row(&entry, entry.path().filename().string().c_str(), last, state.chosen == path, directory, !open))
    {
        state.chosen = path;
    }

    if (tree.toggled())
    {
        if (open) { state.opened.erase(path); } else { state.opened.insert(path); }
    }

    if (!directory || !open)
    {
        return;
    }

    const std::vector<fs::directory_entry> children = listFolder(entry.path());

    tree.push(last);

    for (size_t i = 0; i < children.size(); i++)
    {
        browserRow(tree, state, children[i], i + 1 == children.size(), depth + 1);
    }

    tree.pop();
}

bool fileBrowser(const char* id, BrowserState& state, std::string& picked)
{
    // opens where it was told to, home when nothing said otherwise
    if (state.root[0] == '\0')
    {
        const char* home = std::getenv("HOME");
        std::snprintf(state.root, sizeof(state.root), "%s", home ? home : ".");
    }

    if (!ImGui::IsPopupOpen(id))
    {
        ImGui::OpenPopup(id);
    }

    ImGui::SetNextWindowSize({BROWSER_WIDTH, 0.0f});

    bool taken = false;

    if (ImGui::BeginPopupModal(id, nullptr, ImGuiWindowFlags_NoResize))
    {
        // root is edited as well as walked, so typing it out stays option
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##root", state.root, sizeof(state.root));

        // list is sunken like field above it, both are where input lands
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImGui::GetStyle().FrameRounding);

        ImGui::BeginChild("entries", {0.0f, BROWSER_LIST_HEIGHT});

        TreeView tree;
        tree.setRootless(true);

        const std::vector<fs::directory_entry> entries = listFolder(state.root);

        for (size_t i = 0; i < entries.size(); i++)
        {
            browserRow(tree, state, entries[i], i + 1 == entries.size(), 0);
        }

        ImGui::EndChild();

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        const float actionWidth = std::max(ImGui::CalcTextSize("Cancel").x, ImGui::CalcTextSize("Open").x)
                                + ImGui::GetStyle().FramePadding.x * 2.0f;

        if (ImGui::Button("Cancel", {actionWidth, 0.0f}))
        {
            state.chosen.clear();

            taken = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // nothing marked means root itself is what was meant
        if (ImGui::Button("Open", {actionWidth, 0.0f}))
        {
            picked = state.chosen.empty() ? state.root : state.chosen;
            state.chosen.clear();

            taken = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    return taken;
}

// ---------------------------------------------------------------------------
// asset slots
//
//   Model       Teapot.obj      [...] [Delete]
//               failed to load ...
// ---------------------------------------------------------------------------

// square buttons beside slot, wide enough for what they carry
static float browseButtonWidth()
{
    return ImGui::GetFrameHeight() + ImGui::CalcTextSize("...").x - ImGui::CalcTextSize("\xc3\x97").x;
}

// button beside path field, fills it with whatever browser takes
static bool browseButton(BrowserState& state, char* path, size_t size)
{
    if (ImGui::Button("...##browse", {browseButtonWidth(), 0.0f}))
    {
        state.open = true;
    }

    if (!state.open)
    {
        return false;
    }

    std::string picked;

    if (!fileBrowser(state.title, state, picked))
    {
        return false;
    }

    state.open = false;

    if (picked.empty())
    {
        return false;
    }

    std::snprintf(path, size, "%s", picked.c_str());
    return true;
}

// text that fits width, tail dropped for ellipsis when it does not
static std::string shorten(const char* text, float width)
{
    if (ImGui::CalcTextSize(text).x <= width)
    {
        return text;
    }

    static const char* const TAIL = "...";

    const float room = width - ImGui::CalcTextSize(TAIL).x;
    const char* end = nullptr;

    // font walks string once and says where it ran out of room
    ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), room, 0.0f, text, nullptr, &end);

    return std::string(text, end) + TAIL;
}

// flat slot showing what it holds, says whether drop filled it
//
// reads as place file lands in, not as something to press
static bool assetSlot(const char* text, float width, char* path, size_t size, const char* dragType)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));

    // label changes with what sits in slot, id stays put so neighbours keep theirs
    ImGui::Button((shorten(text, width - ImGui::GetStyle().FramePadding.x * 2.0f) + "##slot").c_str(), {width, 0.0f});

    ImGui::PopStyleColor(3);

    if (!dragType || !ImGui::BeginDragDropTarget())
    {
        return false;
    }

    bool dropped = false;

    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dragType))
    {
        std::snprintf(path, size, "%s", static_cast<const char*>(payload->Data));
        dropped = true;
    }

    ImGui::EndDragDropTarget();
    return dropped;
}

AssetAction assetField(const char* label, const char* current, bool filled, AssetFieldState& state, const char* dragType)
{
    ImGui::PushID(label);

    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    const float browseWidth = browseButtonWidth();
    const float clearWidth = ImGui::CalcTextSize("Delete").x + ImGui::GetStyle().FramePadding.x * 2.0f;

    // what sits in slot, cut when it does not fit so row never grows
    fieldLabel(label);

    const float startX = ImGui::GetCursorPosX();
    // buttons keep their width, slot takes what is left and never goes under
    const float slotWidth = std::max(ImGui::GetContentRegionAvail().x - browseWidth - clearWidth - spacing * 2.0f, MIN_SLOT_WIDTH);

    const bool dropped = assetSlot(current, slotWidth, state.path, sizeof(state.path), dragType);

    ImGui::SameLine(startX + slotWidth + spacing, 0.0f);
    const bool picked = browseButton(state.browser, state.path, sizeof(state.path));

    ImGui::SameLine(0.0f, spacing);

    ImGui::BeginDisabled(!filled);
    const bool cleared = ImGui::Button("Delete##clear", {clearWidth, 0.0f});
    ImGui::EndDisabled();

    errorText(state.error);

    ImGui::PopID();

    if (cleared)
    {
        return AssetAction::Clear;
    }

    return picked || dropped ? AssetAction::Load : AssetAction::None;
}

// ---------------------------------------------------------------------------
// composite
// ---------------------------------------------------------------------------

void splitter(const char* id, float& fraction, float minFraction, float maxFraction)
{
    const float available = ImGui::GetContentRegionAvail().y;

    ImGui::InvisibleButton(id, {ImGui::GetContentRegionAvail().x, SPLITTER_THICKNESS});

    if (ImGui::IsItemActive() && available > 0.0f)
    {
        fraction = glm::clamp(fraction + ImGui::GetIO().MouseDelta.y / available, minFraction, maxFraction);
    }

    if (ImGui::IsItemHovered() || ImGui::IsItemActive())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }

    // line down middle of hit area, brighter while used
    const ImVec2 min = ImGui::GetItemRectMin();
    const ImVec2 max = ImGui::GetItemRectMax();
    const float middle = (min.y + max.y) * 0.5f;

    const glm::vec3 tint = ImGui::IsItemActive() || ImGui::IsItemHovered() ? colors::Grey300 : colors::Grey500;
    ImGui::GetWindowDrawList()->AddLine({min.x, middle}, {max.x, middle},
                                        ImGui::GetColorU32(ImVec4{tint.r, tint.g, tint.b, 0.8f}), 1.0f);
}

} // namespace interface
} // namespace BulletRender
