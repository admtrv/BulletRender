/*
 * main.h
 */

#include "engine/engine.h"
#include "window/window.h"


int main()
{
    window::WindowConfig windowCfg(800, 600, "Demo", true);
    engine::EngineConfig engineCfg(windowCfg);

    engine::Engine::run(engineCfg);

    return 0;
}
