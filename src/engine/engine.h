/*
 * engine.h
 */

#pragma once

#include "window/window.h"
#include "renderer/renderer.h"

namespace engine {

struct EngineConfig {
    window::WindowConfig window;

    explicit EngineConfig(window::WindowConfig w = window::WindowConfig{}) : window(std::move(w)) {}
};

class Engine {
public:
    static void run(const EngineConfig &config);
};

} // namespace engine
