/*
 * Input.cpp
 */

#include "Input.h"

#include "app/Window.h"

namespace BulletRender {
namespace utils {

std::optional<InputKey> toKey(std::string_view name)
{
    static const std::unordered_map<std::string_view, InputKey> KEYS = {
#define INPUT_KEY_NAME(NAME, CODE) {#NAME, InputKey::NAME},
        INPUT_KEYS(INPUT_KEY_NAME)
#undef INPUT_KEY_NAME
    };

    const auto it = KEYS.find(name);
    return it != KEYS.end() ? std::optional<InputKey>(it->second) : std::nullopt;
}

// window owns glfw handle, these pass question along

bool Input::isKeyDown(InputKey key)
{
    return app::Window::isKeyDown(key);
}

bool Input::isMouseDown(MouseButton button)
{
    return app::Window::isMouseDown(button);
}

void Input::getCursorPos(double& x, double& y)
{
    app::Window::getCursorPos(x, y);
}

double Input::consumeScrollDelta()
{
    return app::Window::consumeScrollDelta();
}

// asking watches key from now on, so next frame tells press from hold
bool Input::isKeyPressed(InputKey key)
{
    Input& input = instance();
    const bool held = input.m_keyState.try_emplace(key, false).first->second;

    return isKeyDown(key) && !held;
}

void Input::update()
{
    // bindKey already listed these, so nothing is inserted while they are read
    for (const auto& [key, callback] : m_callbacks)
    {
        if (isKeyPressed(key))
        {
            callback();
        }
    }
}

void Input::endFrame()
{
    // press stands through whole frame, only next one sees hold
    for (auto& [key, held] : m_keyState)
    {
        held = isKeyDown(key);
    }
}

void Input::bindKey(InputKey key, const InputCallback& callback)
{
    m_callbacks[key] = callback;
    m_keyState[key] = false;
}

void Input::unbindKey(InputKey key)
{
    m_callbacks.erase(key);
}

} // namespace utils
} // namespace BulletRender
