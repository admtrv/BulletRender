/*
 * Window.h
 */

#pragma once

#include "Config.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

namespace BulletRender {
namespace app {

struct WindowConfig {
    int width;
    int height;
    std::string title;
    bool resizable;
    bool vsync;         // 1 - vsync on, 0 - vsync off (unlimited fps)

    explicit WindowConfig(int w = 800, int h = 600, std::string t = "BulletRender", bool r = true, bool v = false)
        : width(w), height(h), title(std::move(t)), resizable(r), vsync(v) {}
};

class Window {
public:
    static bool init(const WindowConfig& cfg);
    static void shutdown();
    static void pollEvents();
    static void swapBuffers();
    static bool shouldClose();
    static void setShouldClose(bool value);

    static void getSize(int& width, int& height);


    static GLFWwindow *get();

private:
    static GLFWwindow *s_Window;
};

} // namespace app
} // namespace BulletRender