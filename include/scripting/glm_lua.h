#pragma once

namespace sol {
class state;
}

namespace bee::glm_lua
{
// thanks to Luca!
void bind_vec2(sol::state& lua);
void bind_vec3(sol::state& lua);
void bind_vec4(sol::state& lua);
void bind_quat(sol::state& lua);
void bind_mat2(sol::state& lua);
void bind_mat3(sol::state& lua);
void bind_mat4(sol::state& lua);

// functions
void bind_translate(sol::state& lua);
void bind_rotate(sol::state& lua);

void inline bind(sol::state& lua)
{
    bind_vec2(lua);
    bind_vec3(lua);
    bind_vec4(lua);
    bind_quat(lua);
    bind_mat2(lua);
    bind_mat3(lua);
    bind_mat4(lua);

    bind_translate(lua);
    bind_rotate(lua);
}
} //namespace bee::glm_lua