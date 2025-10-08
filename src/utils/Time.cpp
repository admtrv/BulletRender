/*
 * Time.cpp
 */

#include "Time.h"

namespace luchrender {
namespace utils {

double timeSeconds()
{
    return glfwGetTime();
}

FrameTimer::FrameTimer() : m_last(timeSeconds()) {}

float FrameTimer::tick()
{
    double now = timeSeconds();
    float dt = static_cast<float>(now - m_last);
    m_last = now;
    return dt;
}

} // namespace utils
} // namespace luchrender