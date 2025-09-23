/*
 * window.h
 */

#pragma once

#include "config.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>


namespace window {

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
    static bool init(int width, int height, const char *title, bool resizable);
    static void shutdown();
    static void pollEvents();
    static void swapBuffers();
    static bool shouldClose();

    static GLFWwindow *get();

private:
    static GLFWwindow *s_Window;
};

} // namespace window
