#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"

void bee::glm_lua::bind_mat3(sol::state& lua)
{
    sol::usertype<glm::mat3> mat3_table =
        lua.new_usertype<glm::mat3>("mat3", sol::constructors<glm::mat3(float), glm::mat3()>());

    mat3_table[sol::meta_function::multiplication] =
        sol::overload([](const glm::mat3& m, const glm::vec3& v) { return m * v; },
                      [](const glm::mat3& m1, const glm::mat3& m2) { return m1 * m2; });

    mat3_table["from_quat"] = [](const glm::quat& q) { return glm::mat3(q); };
}
