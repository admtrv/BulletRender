/*
 * Widgets.cpp
 */

#include "interface/elements/Widgets.h"

#include "Colors.h"
#include "Config.h"
#include "render/Material.h"
#include "render/textures/TextureLoader.h"

#include "imgui_stdlib.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>

namespace BulletRender {
namespace interface {

constexpr float TAG_BRIGHTNESS = 1.6f;          // palette tuned for 3d view, tags need more punch on dark ui
constexpr float COLOR_DRAG_SPEED = 0.005f;      // channels live in 0..1, steps must be small
constexpr float SPLITTER_THICKNESS = 6.0f;      // hit area, drawn line is thinner
constexpr float DRAG_RANGE_FRACTION = 0.002f;   // drag speed as share of range, keeps fields feeling alike

constexpr float TEXTURE_PREVIEW_SIZE = 48.0f;
constexpr const char* ASSET_BUTTONS[] = {"Delete", "Load"};     // widest sets the column both share
constexpr int BITS_PER_ROW = 8;                 // mask splits into bytes, rows stay readable
constexpr int AXIS_COUNT = 3;

// wide enough for longest caption, scaled with ui font
float LABEL_COLUMN_WIDTH = LABEL_COLUMN_BASE * config::FontScale;

// three tagged fields, tag painted with color of its component
static bool dragComponents(glm::vec3& value, const char* const names[3], const glm::vec3 tints[3],
                           float speed, float min, float max, const char* format, float width)
{
    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    const float cellWidth = (width - spacing * 2.0f) / 3.0f;

    // widest tag sets offset for all three, otherwise narrower glyph
    // lets its field start earlier and columns drift apart
    float tagWidth = 0.0f;
    for (int component = 0; component < 3; component++)
    {
        tagWidth = std::max(tagWidth, ImGui::CalcTextSize(names[component]).x);
    }
    tagWidth += spacing;

    const float startX = ImGui::GetCursorPosX();

    bool changed = false;

    for (int component = 0; component < 3; component++)
    {
        ImGui::PushID(component);

        // cells placed absolutely, rounding in one does not shift next
        const float cellX = startX + static_cast<float>(component) * (cellWidth + spacing);

        ImGui::SetCursorPosX(cellX);
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(tagColor(tints[component]), "%s", names[component]);

        ImGui::SameLine();
        ImGui::SetCursorPosX(cellX + tagWidth);

        ImGui::SetNextItemWidth(cellWidth - tagWidth);
        changed |= ImGui::DragFloat("##value", &value[component], speed, min, max, format);

        ImGui::PopID();

        if (component < 2)
        {
            ImGui::SameLine();
        }
    }

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

void statRow(const char* label, const char* format, ...)
{
    fieldLabel(label);

    va_list args;
    va_start(args, format);
    ImGui::TextV(format, args);
    va_end(args);
}

// text that fits the width, tail dropped for an ellipsis when it does not
static std::string shorten(const char* text, float width)
{
    if (ImGui::CalcTextSize(text).x <= width)
    {
        return text;
    }

    static const char* const TAIL = "...";

    const float room = width - ImGui::CalcTextSize(TAIL).x;
    const char* end = nullptr;

    // font walks the string once and says where it ran out of room
    ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), room, 0.0f, text, nullptr, &end);

    return std::string(text, end) + TAIL;
}

AssetAction assetField(const char* label, const char* current, bool filled, AssetFieldState& state, const char* dragType)
{
    ImGui::PushID(label);

    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

    float buttonWidth = 0.0f;
    for (const char* caption : ASSET_BUTTONS)
    {
        buttonWidth = std::max(buttonWidth, ImGui::CalcTextSize(caption).x);
    }
    buttonWidth += ImGui::GetStyle().FramePadding.x * 2.0f;

    // what is loaded now, cut when it does not fit so the row never grows
    fieldLabel(label);

    const float startX = ImGui::GetCursorPosX();
    const float valueWidth = ImGui::GetContentRegionAvail().x - buttonWidth - spacing;

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(shorten(current, valueWidth).c_str());

    ImGui::SameLine(startX + valueWidth, spacing);

    ImGui::BeginDisabled(!filled);
    const bool cleared = ImGui::Button("Delete", {buttonWidth, 0.0f});
    ImGui::EndDisabled();

    // path to load another, dropping one loads it at once, row starts where the panel does
    const float pathWidth = ImGui::GetContentRegionAvail().x - buttonWidth - spacing;

    ImGui::SetNextItemWidth(pathWidth);
    ImGui::InputText("##path", state.path, sizeof(state.path));

    bool dropped = false;

    if (dragType && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dragType))
        {
            std::snprintf(state.path, sizeof(state.path), "%s", static_cast<const char*>(payload->Data));
            dropped = true;
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine(0.0f, spacing);
    const bool pressed = ImGui::Button("Load", {buttonWidth, 0.0f});

    errorText(state.error);

    ImGui::PopID();

    if (cleared)
    {
        return AssetAction::Clear;
    }

    return pressed || dropped ? AssetAction::Load : AssetAction::None;
}

bool dragVector3(const char* label, glm::vec3& value, float speed, float min, float max, const char* format)
{
    static const char* const AXIS_NAMES[] = {"x", "y", "z"};
    static const glm::vec3 AXIS_COLORS[] = {colors::AxisX, colors::AxisY, colors::AxisZ};

    ImGui::PushID(label);
    ImGui::TextUnformatted(label);

    const bool changed = dragComponents(value, AXIS_NAMES, AXIS_COLORS, speed, min, max, format,
                                        ImGui::GetContentRegionAvail().x);

    ImGui::PopID();
    return changed;
}

bool checkboxAxes(const char* label, bool& x, bool& y, bool& z)
{
    static const char* const AXIS_NAMES[] = {"x", "y", "z"};
    static const glm::vec3 AXIS_COLORS[] = {colors::AxisX, colors::AxisY, colors::AxisZ};

    bool* const axes[] = {&x, &y, &z};

    ImGui::PushID(label);
    ImGui::TextUnformatted(label);

    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    const float cellWidth = (ImGui::GetContentRegionAvail().x - spacing * 2.0f) / 3.0f;

    // widest tag sets offset for all three, columns line up with vector rows
    float tagWidth = 0.0f;
    for (const char* name : AXIS_NAMES)
    {
        tagWidth = std::max(tagWidth, ImGui::CalcTextSize(name).x);
    }
    tagWidth += spacing;

    const float startX = ImGui::GetCursorPosX();

    bool changed = false;

    for (int axis = 0; axis < AXIS_COUNT; axis++)
    {
        ImGui::PushID(axis);

        const float cellX = startX + static_cast<float>(axis) * (cellWidth + spacing);

        ImGui::SetCursorPosX(cellX);
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(tagColor(AXIS_COLORS[axis]), "%s", AXIS_NAMES[axis]);

        ImGui::SameLine();
        ImGui::SetCursorPosX(cellX + tagWidth);

        changed |= ImGui::Checkbox("##axis", axes[axis]);

        ImGui::PopID();

        if (axis < AXIS_COUNT - 1)
        {
            ImGui::SameLine();
        }
    }

    ImGui::PopID();
    return changed;
}

bool dragColor3(const char* label, glm::vec3& color)
{
    static const char* const CHANNEL_NAMES[] = {"r", "g", "b"};
    static const glm::vec3 CHANNEL_COLORS[] = {colors::Red, colors::Green, colors::Blue};

    ImGui::PushID(label);
    ImGui::TextUnformatted(label);

    // swatch sits at end of row, fields give up its width up front
    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    const float swatchWidth = ImGui::GetFrameHeight();

    bool changed = dragComponents(color, CHANNEL_NAMES, CHANNEL_COLORS, COLOR_DRAG_SPEED, 0.0f, 1.0f, "%.2f",
                                  ImGui::GetContentRegionAvail().x - swatchWidth - spacing);

    ImGui::SameLine(0.0f, spacing);
    changed |= ImGui::ColorEdit3("##swatch", &color.x,
                                 ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

    ImGui::PopID();
    return changed;
}

bool overrideField(const char* label, bool& enabled, glm::vec3& color)
{
    return overrideField(label, enabled, [&color]() { return dragColor3Bare(color); });
}

bool overrideField(const char* label, bool& enabled, float& value, float min, float max, const char* format)
{
    return overrideField(label, enabled, [&]() { return dragScalarBare(value, min, max, format); });
}

void errorText(const std::string& message)
{
    if (message.empty())
    {
        return;
    }

    ImGui::TextColored(tagColor(colors::Red), "%s", message.c_str());
}

bool dragScalarBare(float& value, float min, float max, const char* format)
{
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    const bool changed = ImGui::DragFloat("##value", &value, (max - min) * DRAG_RANGE_FRACTION, min, max, format);

    value = glm::clamp(value, min, max);
    return changed;
}

bool dragColor3Bare(glm::vec3& color)
{
    static const char* const CHANNEL_NAMES[] = {"r", "g", "b"};
    static const glm::vec3 CHANNEL_COLORS[] = {colors::Red, colors::Green, colors::Blue};

    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    const float swatchWidth = ImGui::GetFrameHeight();

    bool changed = dragComponents(color, CHANNEL_NAMES, CHANNEL_COLORS, COLOR_DRAG_SPEED, 0.0f, 1.0f, "%.2f",
                                  ImGui::GetContentRegionAvail().x - swatchWidth - spacing);

    ImGui::SameLine(0.0f, spacing);
    changed |= ImGui::ColorEdit3("##swatch", &color.x,
                                 ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    return changed;
}

void materialTextures(const char* id, render::Material& material, TextureFieldState& state)
{
    ImGui::PushID(id);

    bool anyDrawn = false;

    for (const render::TextureSlot& slot : material.getTextures())
    {
        if (!slot.texture)
        {
            continue;
        }

        anyDrawn = true;
        ImGui::PushID(slot.uniformName.c_str());

        // preview says what it is, uniform name means nothing to user
        ImGui::Image(static_cast<ImTextureID>(slot.texture->id()), {TEXTURE_PREVIEW_SIZE, TEXTURE_PREVIEW_SIZE});
        ImGui::SameLine();

        // size and button share column beside thumbnail
        ImGui::BeginGroup();
        ImGui::Text("%d x %d", slot.texture->getWidth(), slot.texture->getHeight());

        const bool remove = ImGui::Button("Remove");
        ImGui::EndGroup();

        ImGui::PopID();

        if (remove)
        {
            material.clearTexture(slot.uniformName);
            break;                  // the list just changed under us
        }
    }

    // separator needs something above to separate from
    if (anyDrawn)
    {
        ImGui::Separator();
    }
    else
    {
        ImGui::TextDisabled("No textures");
    }

    ImGui::TextUnformatted("Load from file");

    if (loadFromFileField("texture", state.path, sizeof(state.path), "path/to/texture"))
    {
        if (auto texture = render::TextureLoader::instance().load(state.path))
        {
            material.setTexture(render::ALBEDO_UNIFORM, texture, render::ALBEDO_UNIT);
            state.error.clear();
        }
        else
        {
            state.error = "failed to load " + std::string(state.path);
        }
    }

    errorText(state.error);

    ImGui::PopID();
}

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

bool loadFromFileField(const char* id, char* path, size_t size, const char* hint, const char* dragType)
{
    ImGui::PushID(id);

    // button keeps natural width, field takes rest of row
    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    const float buttonWidth = ImGui::CalcTextSize("Load").x + ImGui::GetStyle().FramePadding.x * 2.0f;

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonWidth - spacing);
    ImGui::InputTextWithHint("##path", hint, path, size);

    // a dropped payload fills the field and loads at once, typing still needs the button
    bool dropped = false;

    if (dragType && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dragType))
        {
            const auto* text = static_cast<const char*>(payload->Data);

            std::snprintf(path, size, "%s", text);
            dropped = true;
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine(0.0f, spacing);
    const bool pressed = ImGui::Button("Load");

    ImGui::PopID();
    return pressed || dropped;
}

bool checkboxField(const char* label, bool& value)
{
    ImGui::PushID(label);
    fieldLabel(label);

    const bool changed = ImGui::Checkbox("##value", &value);

    ImGui::PopID();
    return changed;
}

bool dragScalarField(const char* label, float& value, float min, float max, const char* format)
{
    ImGui::PushID(label);
    fieldLabel(label);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    const bool changed = ImGui::DragFloat("##value", &value, (max - min) * DRAG_RANGE_FRACTION, min, max, format);

    ImGui::PopID();

    value = glm::clamp(value, min, max);
    return changed;
}

bool dragScalarField(const char* label, int& value, int min, int max, const char* format)
{
    ImGui::PushID(label);
    fieldLabel(label);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    const bool changed = ImGui::DragInt("##value", &value, static_cast<float>(max - min) * DRAG_RANGE_FRACTION,
                                        min, max, format);

    ImGui::PopID();

    value = glm::clamp(value, min, max);
    return changed;
}

bool inputTextField(const char* label, char* buffer, size_t size, const char* hint)
{
    ImGui::PushID(label);
    fieldLabel(label);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    const bool changed = hint != nullptr ? ImGui::InputTextWithHint("##value", hint, buffer, size)
                                         : ImGui::InputText("##value", buffer, size);

    ImGui::PopID();
    return changed;
}

bool textField(const char* label, std::string& value, const char* hint)
{
    ImGui::PushID(label);
    fieldLabel(label);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    const bool changed = hint != nullptr ? ImGui::InputTextWithHint("##value", hint, &value)
                                         : ImGui::InputText("##value", &value);

    ImGui::PopID();
    return changed;
}

bool comboField(const char* label, int& value, const char* const* options, int count)
{
    ImGui::PushID(label);
    fieldLabel(label);

    const char* current = (value >= 0 && value < count) ? options[value] : "";

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    bool changed = false;

    if (ImGui::BeginCombo("##value", current))
    {
        for (int i = 0; i < count; i++)
        {
            if (ImGui::Selectable(options[i], i == value))
            {
                value = i;
                changed = true;
            }
        }

        ImGui::EndCombo();
    }

    ImGui::PopID();
    return changed;
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

} // namespace interface
} // namespace BulletRender
