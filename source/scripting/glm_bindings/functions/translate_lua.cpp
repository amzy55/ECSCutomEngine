#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"

void bee::glm_lua::bind_translate(sol::state& lua)
{
    lua.set_function("translate", [](const glm::mat4& mat, const glm::vec3& trans) { return glm::translate(mat, trans); });
}