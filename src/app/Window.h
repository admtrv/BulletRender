/*
 * Window.h
 */

#pragma once

#include "Config.h"
#include "utils/Input.h"

#include <string>

struct GLFWwindow;

namespace BulletRender {
namespace app {

enum class CursorMode {
    Normal,     // visible, free to leave the window
    Captured    // hidden and locked, for looking around
};

struct WindowConfig {
    int width;
    int height;
    std::string title;
    bool resizable;
    bool vsync;             // 1 - vsync on, 0 - vsync off (unlimited fps)
    bool preferHighPerfGpu; // hint drivers to pick discrete GPU on hybrid systems

    explicit WindowConfig(int w = 800, int h = 600, std::string t = "BulletRender", bool r = true, bool v = false, bool hp = true)
        : width(w), height(h), title(std::move(t)), resizable(r), vsync(v), preferHighPerfGpu(hp) {}
};

class Window {
public:
    // lifetime
    static bool init(const WindowConfig& cfg);
    static void shutdown();

    static void pollEvents();
    static void swapBuffers();

    static bool shouldClose();
    static void setShouldClose(bool value);

    static bool isHovered();

    // size
    static void getSize(int& width, int& height);
    static void setSize(int width, int height);

    // title
    static const std::string& getTitle() { return s_title; }    // glfw keeps none of its own
    static void setTitle(const std::string& title);

    // resizable
    static bool isResizable();
    static void setResizable(bool resizable);

    // vsync
    static bool isVSync() { return s_vsync; }
    static void setVSync(bool enabled);

    // cursor, captured while looking around
    static void setCursorMode(CursorMode mode);
    static CursorMode getCursorMode();

private:
    friend class Loop;              // imgui binds to the raw handle
    friend class utils::Input;      // user input reaches callers through Input

    // window owns the handle glfw needs, Input is the door for callers
    static bool isKeyDown(utils::InputKey key);
    static bool isMouseDown(utils::MouseButton button);
    static void getCursorPos(double& x, double& y);
    static double consumeScrollDelta();

    static GLFWwindow* get();
    static void scrollCallback(GLFWwindow* w, double xoffset, double yoffset);

    static GLFWwindow* s_Window;
    static std::string s_title;
    static double s_scrollAccum;
    static bool s_vsync;
};

} // namespace app
} // namespace BulletRender