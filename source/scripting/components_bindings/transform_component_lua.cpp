#include <sol/sol.hpp>
#include "scripting/components_lua.h"
#include "ecs/components/TransformComponent.h"

void bee::components_lua::bind_transform(sol::state& lua)
{
    sol::usertype<TransformComponent> transform_table = lua.new_usertype<TransformComponent>("TransformComponent", 
"GetTransformMatrix", &TransformComponent::GetTransformMatrix, 
"GetRotationMatrix", &TransformComponent::GetRotationMatrix,
"GetRotationInRadians", &TransformComponent::GetRotationInRadians,
"GetRotationInDegrees", &TransformComponent::GetRotationInDegrees);

    transform_table["pos"] = &TransformComponent::pos;
    transform_table["rotation"] = &TransformComponent::rotation;
    transform_table["scale"] = &TransformComponent::scale;
}