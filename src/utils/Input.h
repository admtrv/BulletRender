/*
 * Input.h
 */

#pragma once

// glad has to come first wherever the gl headers appear
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace BulletRender {
namespace utils {

// x macro for glfw keys
#define INPUT_KEYS(KEY)                                                                 \
    /* letters */                                                                       \
    KEY(A, A) KEY(B, B) KEY(C, C) KEY(D, D) KEY(E, E) KEY(F, F) KEY(G, G)               \
    KEY(H, H) KEY(I, I) KEY(J, J) KEY(K, K) KEY(L, L) KEY(M, M) KEY(N, N)               \
    KEY(O, O) KEY(P, P) KEY(Q, Q) KEY(R, R) KEY(S, S) KEY(T, T) KEY(U, U)               \
    KEY(V, V) KEY(W, W) KEY(X, X) KEY(Y, Y) KEY(Z, Z)                                   \
                                                                                        \
    /* digits */                                                                        \
    KEY(NUM_0, 0) KEY(NUM_1, 1) KEY(NUM_2, 2) KEY(NUM_3, 3) KEY(NUM_4, 4)               \
    KEY(NUM_5, 5) KEY(NUM_6, 6) KEY(NUM_7, 7) KEY(NUM_8, 8) KEY(NUM_9, 9)               \
                                                                                        \
    /* function */                                                                      \
    KEY(F1, F1) KEY(F2, F2) KEY(F3, F3) KEY(F4, F4) KEY(F5, F5) KEY(F6, F6)             \
    KEY(F7, F7) KEY(F8, F8) KEY(F9, F9) KEY(F10, F10) KEY(F11, F11) KEY(F12, F12)       \
                                                                                        \
    /* arrows */                                                                        \
    KEY(UP, UP) KEY(DOWN, DOWN) KEY(LEFT, LEFT) KEY(RIGHT, RIGHT)                       \
                                                                                        \
    /* modifiers */                                                                     \
    KEY(LEFT_SHIFT, LEFT_SHIFT) KEY(RIGHT_SHIFT, RIGHT_SHIFT)                           \
    KEY(LEFT_CONTROL, LEFT_CONTROL) KEY(RIGHT_CONTROL, RIGHT_CONTROL)                   \
    KEY(LEFT_ALT, LEFT_ALT) KEY(RIGHT_ALT, RIGHT_ALT)                                   \
                                                                                        \
    /* editing */                                                                       \
    KEY(SPACE, SPACE) KEY(ENTER, ENTER) KEY(TAB, TAB) KEY(BACKSPACE, BACKSPACE)         \
    KEY(DELETE, DELETE) KEY(INSERT, INSERT) KEY(ESCAPE, ESCAPE)                         \
                                                                                        \
    /* navigation */                                                                    \
    KEY(HOME, HOME) KEY(END, END) KEY(PAGE_UP, PAGE_UP) KEY(PAGE_DOWN, PAGE_DOWN)       \
                                                                                        \
    /* punctuation */                                                                   \
    KEY(MINUS, MINUS) KEY(EQUAL, EQUAL) KEY(COMMA, COMMA) KEY(PERIOD, PERIOD)           \
    KEY(SLASH, SLASH) KEY(SEMICOLON, SEMICOLON) KEY(APOSTROPHE, APOSTROPHE)             \
    KEY(LEFT_BRACKET, LEFT_BRACKET) KEY(RIGHT_BRACKET, RIGHT_BRACKET)                   \
    KEY(BACKSLASH, BACKSLASH) KEY(GRAVE, GRAVE_ACCENT)                                  \
                                                                                        \
    /* keypad */                                                                        \
    KEY(KEYPAD_0, KP_0) KEY(KEYPAD_1, KP_1) KEY(KEYPAD_2, KP_2) KEY(KEYPAD_3, KP_3)     \
    KEY(KEYPAD_4, KP_4) KEY(KEYPAD_5, KP_5) KEY(KEYPAD_6, KP_6) KEY(KEYPAD_7, KP_7)     \
    KEY(KEYPAD_8, KP_8) KEY(KEYPAD_9, KP_9)                                             \
    KEY(KEYPAD_ADD, KP_ADD) KEY(KEYPAD_SUBTRACT, KP_SUBTRACT)                           \
    KEY(KEYPAD_MULTIPLY, KP_MULTIPLY) KEY(KEYPAD_DIVIDE, KP_DIVIDE)                     \
    KEY(KEYPAD_DECIMAL, KP_DECIMAL) KEY(KEYPAD_ENTER, KP_ENTER)

enum class InputKey : int {
#define INPUT_KEY_ENUM(NAME, CODE) NAME = GLFW_KEY_##CODE,
    INPUT_KEYS(INPUT_KEY_ENUM)
#undef INPUT_KEY_ENUM
};

std::optional<InputKey> toKey(std::string_view name);

enum class MouseButton {
    Left,
    Right,
    Middle
};

using InputCallback = std::function<void()>;

class Input {
public:
    static Input& instance() {
        static Input m;
        return m;
    }

    // state, asked any time
    static bool isKeyDown(InputKey key);
    static bool isKeyPressed(InputKey key);     // down this frame, not one before
    static bool isMouseDown(MouseButton button);
    static void getCursorPos(double& x, double& y);

    // wheel travel since last call, taken and cleared
    static double consumeScrollDelta();

    // callbacks, fired on press itself
    void update();

    // closes frame, until it runs every caller still sees press
    void endFrame();

    void bindKey(InputKey key, const InputCallback& callback);
    void unbindKey(InputKey key);

private:
    Input() = default;
    ~Input() = default;

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    std::unordered_map<InputKey, InputCallback> m_callbacks;

    // what every asked key did last frame, so press is told from hold
    std::unordered_map<InputKey, bool> m_keyState;
};

} // namespace utils
} // namespace BulletRender
