#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sol/sol.hpp>
#include "scripting/glm_lua.h"

void bee::glm_lua::bind_quat(sol::state& lua)
{
    sol::usertype<glm::quat> quat_table =
        lua.new_usertype<glm::quat>("quat", sol::constructors<glm::quat(float, float, float, float), glm::quat()>());

    quat_table["x"] = &glm::quat::x;
    quat_table["y"] = &glm::quat::y;
    quat_table["z"] = &glm::quat::z;
    quat_table["w"] = &glm::quat::w;

    quat_table[sol::meta_function::multiplication] = [](const glm::quat& q1, const glm::quat& q2) { return q1 * q2; };
    quat_table[sol::meta_function::unary_minus] = [](const glm::quat& q) { return glm::inverse(q); };
    quat_table[sol::meta_function::equal_to] = std::equal_to<glm::quat>();

    quat_table["length"] = [](const glm::quat& q) { return glm::length(q); };
    quat_table["normalize"] = [](const glm::quat& q) { return glm::normalize(q); };
    quat_table["conjugate"] = [](const glm::quat& q) { return glm::conjugate(q); };
    quat_table["eulerAngles"] = [](const glm::quat& q) { return glm::eulerAngles(q); };
    quat_table["angleAxis"] = [](const float angle, const glm::vec3& axis) { return glm::angleAxis(angle, axis); };
    quat_table["rotate"] = [](const glm::quat& q, const float angle, const glm::vec3& axis) { return glm::rotate(q, angle, axis); };
    quat_table["mix"] = [](const glm::quat& q1, const glm::quat& q2, float a) { return glm::mix(q1, q2, a); };
    quat_table["slerp"] = [](const glm::quat& q1, const glm::quat& q2, float a) { return glm::slerp(q1, q2, a); };
    quat_table["conjugate"] = [](const glm::quat& q) { return glm::conjugate(q); };
    quat_table["inverse"] = [](const glm::quat& q) { return glm::inverse(q); };
    quat_table["to_mat4"] = [](const glm::quat& q) { return glm::mat4_cast(q); };
    quat_table["to_mat4"] = [](const glm::quat& q) { return glm::mat3_cast(q); };
    quat_table["identity"] = []() { return glm::quat(); };
    quat_table["from_mat4"] = [](const glm::mat4& m) { return glm::quat_cast(m); };
    quat_table["from_mat3"] = [](const glm::mat3& m) { return glm::quat_cast(m); };
}
