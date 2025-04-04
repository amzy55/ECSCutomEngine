#pragma once
#include "BaseSystem.h"

namespace bee
{
struct PhysicsBody2DComponent;
struct CollisionData;

class PhysicsSystem2D final : public BaseSystem
{
public:
    PhysicsSystem2D(entt::registry& registry);

    void Update(const float& dt) override;

private:
    void UpdateTransforms(float dt);
    void CheckAndRegisterCollisions();
    void DebugDrawing();

    bool CheckAndRegisterCollision(
        const entt::entity& entity1, PhysicsBody2DComponent& body1, const entt::entity& entity2, PhysicsBody2DComponent& body2,
        CollisionData& collision);
};

} //namespace bee