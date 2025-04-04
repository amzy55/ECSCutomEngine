#include <glm/glm.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"

void bee::glm_lua::bind_vec4(sol::state& lua)
{
    sol::usertype<glm::vec4> vec4_table =
        lua.new_usertype<glm::vec4>("vec4", sol::constructors<glm::vec4(float, float, float, float), glm::vec4()>());

    vec4_table["x"] = &glm::vec4::x;
    vec4_table["y"] = &glm::vec4::y;
    vec4_table["z"] = &glm::vec4::z;
    vec4_table["w"] = &glm::vec4::w;
    vec4_table[sol::meta_function::addition] = [](const glm::vec4& v1, const glm::vec4& v2) { return v1 + v2; };
    vec4_table[sol::meta_function::subtraction] = [](const glm::vec4& v1, const glm::vec4& v2) { return v1 - v2; };
    vec4_table[sol::meta_function::multiplication] = sol::overload(
        [](const glm::vec4& v, const float scalar) { return v * scalar; },
        [](const glm::vec4& v1, const glm::vec4& v2) { return v1 * v2; });
    vec4_table[sol::meta_function::division] = [](const glm::vec4& v, const float scalar) { return v / scalar; };
    vec4_table[sol::meta_function::unary_minus] = [](const glm::vec4& v) { return -v; };
    vec4_table[sol::meta_function::equal_to] = [](const glm::vec4& v1, const glm::vec4& v2) { return v1 == v2; };

    vec4_table["length"] = [](const glm::vec4& v) { return glm::length(v); };
    vec4_table["normalize"] = [](const glm::vec4& v) { return glm::normalize(v); };
    vec4_table["dot"] = [](const glm::vec4& lhs, const glm::vec4& rhs) { return glm::dot(lhs, rhs); };
}
