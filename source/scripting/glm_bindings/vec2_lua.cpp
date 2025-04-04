#include <glm/glm.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"

void bee::glm_lua::bind_vec2(sol::state& lua)
{
    sol::usertype<glm::vec2> vec2_table =
        lua.new_usertype<glm::vec2>("vec2", sol::constructors<glm::vec2(float, float), glm::vec2()>());

    vec2_table["x"] = &glm::vec2::x;
    vec2_table["y"] = &glm::vec2::y;
    vec2_table[sol::meta_function::addition] = [](const glm::vec2& v1, const glm::vec2& v2) { return v1 + v2; };
    vec2_table[sol::meta_function::subtraction] = [](const glm::vec2& v1, const glm::vec2& v2) { return v1 - v2; };
    vec2_table[sol::meta_function::multiplication] = sol::overload(
        [](const glm::vec2& v, const float scalar) { return v * scalar; },
        [](const glm::vec2& v1, const glm::vec2& v2) { return v1 * v2; });
    vec2_table[sol::meta_function::division] = [](const glm::vec2& v, const float scalar) { return v / scalar; };
    vec2_table[sol::meta_function::unary_minus] = [](const glm::vec2& v) { return -v; };
    vec2_table[sol::meta_function::equal_to] = [](const glm::vec2& v1, const glm::vec2& v2) { return v1 == v2; };

    vec2_table["length"] = [](const glm::vec2& v) { return glm::length(v); };
    vec2_table["normalize"] = [](const glm::vec2& v) { return glm::normalize(v); };
    vec2_table["dot"] = [](const glm::vec2& lhs, const glm::vec2& rhs) { return glm::dot(lhs, rhs); };
}
