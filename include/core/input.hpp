#pragma once

#include <glm/vec2.hpp>
#include <unordered_map>

namespace bee
{
/// <summary>
/// The Input class provides a cross-platform way to handle input from keyboard, mouse, and gamepads.
/// </summary>
class Input
{
public:
    /// <summary>
    /// An enum listing all supported keyboard keys.
    /// This uses the same numbering as in GLFW input, so a GLFW-based implementation can use it directly without any further
    /// mapping.
    /// </summary>
    enum class KeyboardKey
    {
        Space = 32,
        Apostrophe = 39,
        Comma = 44,
        Minus = 45,
        Period = 46,
        Slash = 47,
        Digit0 = 48,
        Digit1 = 49,
        Digit2 = 50,
        Digit3 = 51,
        Digit4 = 52,
        Digit5 = 53,
        Digit6 = 54,
        Digit7 = 55,
        Digit8 = 56,
        Digit9 = 57,
        Semicolon = 59,
        Equal = 61,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,
        Backslash = 92,
        RightBracket = 93,
        GraveAccent = 96,
        World1 = 161,
        World2 = 162,
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        ArrowRight = 262,
        ArrowLeft = 263,
        ArrowDown = 264,
        ArrowUp = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        Numpad0 = 320,
        Numpad1 = 321,
        Numpad2 = 322,
        Numpad3 = 323,
        Numpad4 = 324,
        Numpad5 = 325,
        Numpad6 = 326,
        Numpad7 = 327,
        Numpad8 = 328,
        Numpad9 = 329,
        NumpadDecimal = 330,
        NumpadDivide = 331,
        NumpadMultiply = 332,
        NumpadSubtract = 333,
        NumpadAdd = 334,
        NumpadEnter = 335,
        NumpadEqual = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
    };

    /// <summary>
    /// An enum listing all supported gamepad axes with analog input values.
    /// This uses the same numbering as in GLFW input, so a GLFW-based implementation can use it directly without any further
    /// mapping.
    /// </summary>
    enum class GamepadAxis
    {
        /// Represents the horizontal axis of the left gamepad stick, with an analog input value between -1 (left) and 1
        /// (right).
        StickLeftX = 0,
        /// Represents the vertical axis of the left gamepad stick, with an analog input value between -1 (down) and 1 (up).
        StickLeftY = 1,
        /// Represents the horizontal axis of the right gamepad stick, with an analog input value between -1 (left) and 1
        /// (right).
        StickRightX = 2,
        /// Represents the vertical axis of the right gamepad stick, with an analog input value between -1 (down) and 1 (up).
        StickRightY = 3,
        /// Represents the left trigger of a gamepad, with an analog input value between 0 (not pressed) and 1 (fully pressed).
        TriggerLeft = 4,
        /// Represents the right trigger of a gamepad, with an analog input value between 0 (not pressed) and 1 (fully pressed).
        TriggerRight = 5
    };

    /// <summary>
    /// An enum listing all supported gamepad buttons with digital input values.
    /// This uses the same numbering as in GLFW input, so a GLFW-based implementation can use it directly without any further
    /// mapping.
    /// </summary>
    enum class GamepadButton
    {
        /// Represents the bottom (south) button of the 4 main action buttons on a gamepad.
        South = 0,
        /// Represents the right (east) button of the 4 main action buttons on a gamepad.
        East = 1,
        /// Represents the left (west) button of the 4 main action buttons on a gamepad.
        West = 2,
        /// Represents the top (north) button of the 4 main action buttons on a gamepad.
        North = 3,

        /// Represents the left shoulder button on a gamepad.
        ShoulderLeft = 4,
        /// Represents the right shoulder button on a gamepad.
        ShoulderRight = 5,

        /// Represents the left of the two menu-related buttons on a gamepad.
        /// This button has different names on different platforms, such as Share (Xbox) or - (Switch). The PS5 does not have
        /// such a button.
        MenuLeft = 6,
        /// Represents the right of the two menu-related buttons on a gamepad.
        /// This button has different names on different platforms, such as Menu (Xbox), Start (PS5), or + (Switch).
        MenuRight = 7,

        // Button 8 is not used, so that we have a 1-on-1 mapping with the GLFW enum.

        /// Represents the pressing of the left gamepad stick.
        StickPressLeft = 9,
        /// Represents the pressing of the right gamepad stick.
        StickPressRight = 10,

        /// Represents the up arrow of the D-pad on a gamepad.
        DPadUp = 11,
        /// Represents the right arrow of the D-pad on a gamepad.
        DPadRight = 12,
        /// Represents the down arrow of the D-pad on a gamepad.
        DPadDown = 13,
        /// Represents the left arrow of the D-pad on a gamepad.
        DPadLeft = 14
    };

    /// <summary>
    /// An enum listing all supported mouse buttons.
    /// This uses the same numbering as in GLFW input, so a GLFW-based implementation can use it directly without any further
    /// mapping.
    /// </summary>
    enum class MouseButton
    {
        Left = 0,
        Right = 1,
        Middle = 2
    };

    /// Returns the current floating-point value of a given gamepad axis.
    float GetGamepadAxis(int gamepadID, GamepadAxis axis) const;

    /// Checks and returns whether a gamepad with a given ID is currently available for input.
    bool IsGamepadAvailable(int gamepadID) const;

    /// Checks and returns whether a given gamepad button is being held down in the current frame.
    bool GetGamepadButton(int gamepadID, GamepadButton button) const;

    /// Checks and returns whether a given gamepad button is being pressed in the current frame without having been pressed in
    /// the previous frame.
    bool GetGamepadButtonOnce(int gamepadID, GamepadButton button) const;

    /// Checks and returns whether a mouse is currently available for input.
    bool IsMouseAvailable() const;

    /// Checks and returns whether a given mouse button is being held down in the current frame.
    bool GetMouseButton(MouseButton button) const;

    /// Checks and returns whether a given mouse button is being pressed in the current frame without having been pressed in the
    /// previous frame.
    bool GetMouseButtonOnce(MouseButton button) const;

    /// Gets the screen position of the mouse, relative to the top-left corner of the screen.
    glm::vec2 GetMousePosition() const;

    /// Gets the amount that the mouse has moved in the last frame.
    glm::vec2 GetMouseDelta() const;

    /// Gets the mouse wheel, relative to the initial value when starting the game.
    float GetMouseWheel() const;

    /// Checks and returns whether a keyboard is currently available for input.
    bool IsKeyboardAvailable() const;

    /// Checks and returns whether a given keyboard key is being held down in the current frame.
    bool GetKeyboardKey(KeyboardKey button) const;

    /// Checks and returns whether a given keyboard key is being pressed in the current frame without having been pressed in the
    /// previous frame.
    bool GetKeyboardKeyOnce(KeyboardKey button) const;

    Input();
    ~Input();

    struct InputToStrings
    {
        std::unordered_map<KeyboardKey, std::string> keyboardKeys = {
            {KeyboardKey::Space, "Space"},
            {KeyboardKey::Apostrophe, "Apostrophe"},
            {KeyboardKey::Comma, "Comma"},
            {KeyboardKey::Minus, "Minus"},
            {KeyboardKey::Period, "Period"},
            {KeyboardKey::Slash, "Slash"},
            {KeyboardKey::Digit0, "Digit0"},
            {KeyboardKey::Digit1, "Digit1"},
            {KeyboardKey::Digit2, "Digit2"},
            {KeyboardKey::Digit3, "Digit3"},
            {KeyboardKey::Digit4, "Digit4"},
            {KeyboardKey::Digit5, "Digit5"},
            {KeyboardKey::Digit6, "Digit6"},
            {KeyboardKey::Digit7, "Digit7"},
            {KeyboardKey::Digit8, "Digit8"},
            {KeyboardKey::Digit9, "Digit9"},
            {KeyboardKey::Semicolon, "Semicolon"},
            {KeyboardKey::Equal, "Equal"},
            {KeyboardKey::A, "A"},
            {KeyboardKey::B, "B"},
            {KeyboardKey::C, "C"},
            {KeyboardKey::D, "D"},
            {KeyboardKey::E, "E"},
            {KeyboardKey::F, "F"},
            {KeyboardKey::G, "G"},
            {KeyboardKey::H, "H"},
            {KeyboardKey::I, "I"},
            {KeyboardKey::J, "J"},
            {KeyboardKey::K, "K"},
            {KeyboardKey::L, "L"},
            {KeyboardKey::M, "M"},
            {KeyboardKey::N, "N"},
            {KeyboardKey::O, "O"},
            {KeyboardKey::P, "P"},
            {KeyboardKey::Q, "Q"},
            {KeyboardKey::R, "R"},
            {KeyboardKey::S, "S"},
            {KeyboardKey::T, "T"},
            {KeyboardKey::U, "U"},
            {KeyboardKey::V, "V"},
            {KeyboardKey::W, "W"},
            {KeyboardKey::X, "X"},
            {KeyboardKey::Y, "Y"},
            {KeyboardKey::Z, "Z"},
            {KeyboardKey::LeftBracket, "LeftBracket"},
            {KeyboardKey::Backslash, "Backslash"},
            {KeyboardKey::RightBracket, "RightBracket"},
            {KeyboardKey::GraveAccent, "GraveAccent"},
            {KeyboardKey::World1, "World1"},
            {KeyboardKey::World2, "World2"},
            {KeyboardKey::Escape, "Escape"},
            {KeyboardKey::Enter, "Enter"},
            {KeyboardKey::Tab, "Tab"},
            {KeyboardKey::Backspace, "Backspace"},
            {KeyboardKey::Insert, "Insert"},
            {KeyboardKey::Delete, "Delete"},
            {KeyboardKey::ArrowRight, "ArrowRight"},
            {KeyboardKey::ArrowLeft, "ArrowLeft"},
            {KeyboardKey::ArrowDown, "ArrowDown"},
            {KeyboardKey::ArrowUp, "ArrowUp"},
            {KeyboardKey::PageUp, "PageUp"},
            {KeyboardKey::PageDown, "PageDown"},
            {KeyboardKey::Home, "Home"},
            {KeyboardKey::End, "End"},
            {KeyboardKey::CapsLock, "CapsLock"},
            {KeyboardKey::ScrollLock, "ScrollLock"},
            {KeyboardKey::NumLock, "NumLock"},
            {KeyboardKey::PrintScreen, "PrintScreen"},
            {KeyboardKey::Pause, "Pause"},
            {KeyboardKey::F1, "F1"},
            {KeyboardKey::F2, "F2"},
            {KeyboardKey::F3, "F3"},
            {KeyboardKey::F4, "F4"},
            {KeyboardKey::F5, "F5"},
            {KeyboardKey::F6, "F6"},
            {KeyboardKey::F7, "F7"},
            {KeyboardKey::F8, "F8"},
            {KeyboardKey::F9, "F9"},
            {KeyboardKey::F10, "F10"},
            {KeyboardKey::F11, "F11"},
            {KeyboardKey::F12, "F12"},
            {KeyboardKey::F13, "F13"},
            {KeyboardKey::F14, "F14"},
            {KeyboardKey::F15, "F15"},
            {KeyboardKey::F16, "F16"},
            {KeyboardKey::F17, "F17"},
            {KeyboardKey::F18, "F18"},
            {KeyboardKey::F19, "F19"},
            {KeyboardKey::F20, "F20"},
            {KeyboardKey::F21, "F21"},
            {KeyboardKey::F22, "F22"},
            {KeyboardKey::F23, "F23"},
            {KeyboardKey::F24, "F24"},
            {KeyboardKey::F25, "F25"},
            {KeyboardKey::Numpad0, "Numpad0"},
            {KeyboardKey::Numpad1, "Numpad1"},
            {KeyboardKey::Numpad2, "Numpad2"},
            {KeyboardKey::Numpad3, "Numpad3"},
            {KeyboardKey::Numpad4, "Numpad4"},
            {KeyboardKey::Numpad5, "Numpad5"},
            {KeyboardKey::Numpad6, "Numpad6"},
            {KeyboardKey::Numpad7, "Numpad7"},
            {KeyboardKey::Numpad8, "Numpad8"},
            {KeyboardKey::Numpad9, "Numpad9"},
            {KeyboardKey::NumpadDecimal, "NumpadDecimal"},
            {KeyboardKey::NumpadDivide, "NumpadDivide"},
            {KeyboardKey::NumpadMultiply, "NumpadMultiply"},
            {KeyboardKey::NumpadSubtract, "NumpadSubtract"},
            {KeyboardKey::NumpadAdd, "NumpadAdd"},
            {KeyboardKey::NumpadEnter, "NumpadEnter"},
            {KeyboardKey::NumpadEqual, "NumpadEqual"},
            {KeyboardKey::LeftShift, "LeftShift"},
            {KeyboardKey::LeftControl, "LeftControl"},
            {KeyboardKey::LeftAlt, "LeftAlt"},
            {KeyboardKey::LeftSuper, "LeftSuper"},
            {KeyboardKey::RightShift, "RightShift"},
            {KeyboardKey::RightControl, "RightControl"},
            {KeyboardKey::RightAlt, "RightAlt"},
            {KeyboardKey::RightSuper, "RightSuper"},
            {KeyboardKey::Menu, "Menu"}};

        std::unordered_map<GamepadButton, std::string> controllerButtons = {
            {GamepadButton::South, "A/X"},
            {GamepadButton::East, "B/Circle"},
            {GamepadButton::West, "X/Square"},
            {GamepadButton::North, "Y/Triangle"},
            {GamepadButton::ShoulderLeft, "ShoulderLeft"},
            {GamepadButton::ShoulderRight, "ShoulderRight"},
            {GamepadButton::MenuLeft, "MenuLeft"},
            {GamepadButton::MenuRight, "MenuRight"},
            {GamepadButton::StickPressLeft, "StickPressLeft"},
            {GamepadButton::StickPressRight, "StickPressRight"},
            {GamepadButton::DPadUp, "DPadUp"},
            {GamepadButton::DPadRight, "DPadRight"},
            {GamepadButton::DPadDown, "DPadDown"},
            {GamepadButton::DPadLeft, "DPadLeft"}};
    }inputToStrings;

private:
    friend class EngineClass;
    void Update();
};

} // namespace bee
