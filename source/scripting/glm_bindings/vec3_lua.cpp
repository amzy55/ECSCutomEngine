#include <glm/glm.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"

void bee::glm_lua::bind_vec3(sol::state& lua)
{
    sol::usertype<glm::vec3> vec3_table =
        lua.new_usertype<glm::vec3>("vec3", sol::constructors<glm::vec3(float, float, float), glm::vec3()>());

    vec3_table["x"] = &glm::vec3::x;
    vec3_table["y"] = &glm::vec3::y;
    vec3_table["z"] = &glm::vec3::z;
    vec3_table[sol::meta_function::addition] = [](const glm::vec3& v1, const glm::vec3& v2) { return v1 + v2; };
    vec3_table[sol::meta_function::subtraction] = [](const glm::vec3& v1, const glm::vec3& v2) { return v1 - v2; };
    vec3_table[sol::meta_function::multiplication] = sol::overload(
        [](const glm::vec3& v, const float scalar) { return v * scalar; },
        [](const glm::vec3& v1, const glm::vec3& v2) { return v1 * v2; });
    vec3_table[sol::meta_function::division] = [](const glm::vec3& v, const float scalar) { return v / scalar; };
    vec3_table[sol::meta_function::unary_minus] = [](const glm::vec3& v) { return -v; };
    vec3_table[sol::meta_function::equal_to] = [](const glm::vec3& v1, const glm::vec3& v2) { return v1 == v2; };

    vec3_table["length"] = [](const glm::vec3& v) { return glm::length(v); };
    vec3_table["normalize"] = [](const glm::vec3& v) { return glm::normalize(v); };
    vec3_table["dot"] = [](const glm::vec3& lhs, const glm::vec3& rhs) { return glm::dot(lhs, rhs); };
    vec3_table["cross"] = [](const glm::vec3& lhs, const glm::vec3& rhs) { return glm::cross(lhs, rhs); };
}
