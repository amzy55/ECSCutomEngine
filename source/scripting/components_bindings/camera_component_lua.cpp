#include <sol/sol.hpp>
#include "scripting/components_lua.h"
#include "ecs/components/CameraComponent.h"

void bee::components_lua::bind_camera(sol::state& lua)
{
    sol::usertype<CameraComponent> camera_table = lua.new_usertype<CameraComponent>("CameraComponent");

    camera_table["projection"] = &CameraComponent::projection;
    camera_table["view"] = &CameraComponent::view;
    camera_table["speed"] = &CameraComponent::speed;
}