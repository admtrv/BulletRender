/*
 * time.h
 */

#pragma once

#include <GLFW/glfw3.h>

namespace luchrender {
namespace utils {

// current time in seconds since the application started
double timeSeconds();

class FrameTimer {
public:
    FrameTimer();
    // returns dt in seconds since the last call
    float tick();
private:
    double m_last;
};

} // namespace utils
} // namespace luchrender