/*
 * window.h
 */

#pragma once

#include "config.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

namespace luchrender {
namespace app {

struct WindowConfig {
    int width;
    int height;
    std::string title;
    bool resizable;

    explicit WindowConfig(int w = 800, int h = 600, std::string t = "luchrender", bool r = true)
        : width(w), height(h), title(std::move(t)), resizable(r) {}
};

class Window {
public:
    static bool init(const WindowConfig& cfg);
    static void shutdown();
    static void pollEvents();
    static void swapBuffers();
    static bool shouldClose();

    static void getSize(int& width, int& height);


    static GLFWwindow *get();

private:
    static GLFWwindow *s_Window;
};

} // namespace app
} // namespace luchrender