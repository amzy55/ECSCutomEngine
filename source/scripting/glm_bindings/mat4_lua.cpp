#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"

void bee::glm_lua::bind_mat4(sol::state& lua)
{
    sol::usertype<glm::mat4> mat4_table =
        lua.new_usertype<glm::mat4>("mat4", sol::constructors<glm::mat4(float), glm::mat4()>());

    mat4_table[sol::meta_function::multiplication] =
        sol::overload([](const glm::mat4& m, const glm::vec4& v) { return m * v; },
                      [](const glm::mat4& m1, const glm::mat4& m2) { return m1 * m2; });

    mat4_table["from_quat"] = [](const glm::quat& q) { return glm::mat4(q); };

    mat4_table["look_at"] = [](const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
    { return glm::lookAt(eye, center, up); };

    mat4_table["perspective"] = [](float fovy, float aspect, float zNear, float zFar)
    { return glm::perspective(fovy, aspect, zNear, zFar); };

    mat4_table["ortho"] = [](float left, float right, float bottom, float top, float zNear, float zFar)
    { return glm::ortho(left, right, bottom, top, zNear, zFar); };
}