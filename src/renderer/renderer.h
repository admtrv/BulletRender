/*
 * renderer.h
 */

#pragma once

namespace renderer {

class Renderer {
public:
    static void init();
    static void clear(float r, float g, float b, float a);
    static void beginFrame();
    static void endFrame();
};

} // namespace renderer
