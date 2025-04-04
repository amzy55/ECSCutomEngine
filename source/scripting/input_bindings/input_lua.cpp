#include <sol/sol.hpp>
#include "scripting/input_lua.h"

#include "core/engine.hpp"
#include "core/input.hpp"

void bee::input_lua::bind(sol::state& lua)
{
    auto keyboard_enum = lua.new_enum(
        "KeyboardKey",
        "W",Input::KeyboardKey::W, 
        "A", Input::KeyboardKey::A, 
        "S", Input::KeyboardKey::S, 
        "D",Input::KeyboardKey::D, 
        "Q", Input::KeyboardKey::Q,
        "E", Input::KeyboardKey::E, 
        "Space", Input::KeyboardKey::Space);

    lua.set_function("GetKeyboardKey", [](const Input::KeyboardKey key) { return Engine.Input().GetKeyboardKey(key); });

    auto mouse_enum = lua.new_enum(
        "MouseButton", 
        "Left", Input::MouseButton::Left, 
        "Right", Input::MouseButton::Right, 
        "Middle", Input::MouseButton::Middle);

    lua.set_function("GetMouseButton", [](const Input::MouseButton button) { return Engine.Input().GetMouseButton(button); });

    lua.set_function("GetMouseDelta", []() { return Engine.Input().GetMouseDelta(); });
}
