#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bee
{

struct TransformComponent
{
    // local
    glm::vec3 pos = glm::vec3(0.f);
    glm::quat rotation = {1.f, 0.f, 0.f, 0.f};
    glm::vec3 scale = glm::vec3(1.f);

    [[nodiscard]] glm::mat4 GetTransformMatrix() const
    {
        glm::mat4 matrix = glm::mat4(1.f);
        matrix = glm::translate(matrix, pos);
        matrix *= GetRotationMatrix();
        matrix = glm::scale(matrix, scale);
        return matrix;
    }
    [[nodiscard]] glm::mat4 GetRotationMatrix() const { return glm::mat4_cast(rotation); }
    [[nodiscard]] glm::vec3 GetRotationInRadians() const { return glm::eulerAngles(rotation); }
    [[nodiscard]] glm::vec3 GetRotationInDegrees() const { return glm::degrees(GetRotationInRadians()); }

    // world
    glm::mat4 worldTransform = glm::mat4(1.f);
};

}
