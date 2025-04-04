#include <glm/glm.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"

void bee::glm_lua::bind_mat2(sol::state& lua)
{
    sol::usertype<glm::mat2> mat2_table =
        lua.new_usertype<glm::mat2>("mat2", sol::constructors<glm::mat2(float), glm::mat2()>());

    mat2_table[sol::meta_function::multiplication] =
        sol::overload([](const glm::mat2& m, const glm::vec2& v) { return m * v; },
                      [](const glm::mat2& m1, const glm::mat2& m2) { return m1 * m2; });
}
