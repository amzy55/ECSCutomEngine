#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"
#include "tools/tools.hpp"

void bee::glm_lua::bind_rotate(sol::state& lua)
{
    lua.set_function(
        "rotate", [](const glm::mat4& mat, const float angle, const glm::vec3 axis) { return glm::rotate(mat, angle, axis); });

    lua.set_function(
        "GetRotationInRadiansFromMatrix", [](const glm::mat4& mat) { return GetRotationInRadiansFromMatrix(mat); });

    lua.set_function(
        "degrees", [](const float radians) { return glm::degrees(radians); });
}
