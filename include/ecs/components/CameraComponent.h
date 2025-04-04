#pragma once
#include <glm/mat4x4.hpp>

namespace bee
{

struct CameraComponent
{
    glm::mat4 projection = glm::mat4(1.f);
    glm::mat4 view = glm::mat4(1.f);
    float speed = 25.f;
};

}
