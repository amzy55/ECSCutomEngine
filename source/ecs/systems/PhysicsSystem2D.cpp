#include "ecs/systems/PhysicsSystem2D.h"

#include <glm/vec2.hpp>
#include <glm/gtx/norm.hpp>

#include "ecs/components/AbilityComponent.h"
#include "ecs/components/PhysicsBody2DComponent.h"
#include "ecs/components/TransformComponent.h"

#ifdef _DEBUG
#include "core/engine.hpp"
#include "rendering/debug_render.hpp"
#endif


bee::PhysicsSystem2D::PhysicsSystem2D(entt::registry& registry)
    : BaseSystem(registry) {}

void bee::PhysicsSystem2D::Update(const float& dt)
{
    UpdateTransforms(dt);
    CheckAndRegisterCollisions();
    DebugDrawing();
}

void bee::PhysicsSystem2D::UpdateTransforms(const float dt)
{
    auto view = m_registry.view<PhysicsBody2DComponent, TransformComponent>();
    for (auto [entity, body, transformComponent] : view.each())
    {
        // Clear collision data and update position based on velocity
        body.Update(dt);

        // synchronize transforms with physics bodies for rendering
        // position
        transformComponent.pos = {
            body.position.x,
            transformComponent.pos.y,
            body.position.y,
        };

        // rotation / orientation
        if (body.velocity != glm::vec2(0.f, 0.f))
        {
            const glm::vec2 direction2D = glm::normalize(body.velocity);
            glm::vec3 direction3D(direction2D.x, 0.0f, direction2D.y);
            // Calculate angle based on direction
            float angle = acos(glm::dot(glm::normalize(direction3D), {0.0f, 0.0f, 1.0f}));
            // Determine the rotation direction (clockwise or counterclockwise)
            if (direction3D.x < 0)
                angle = -angle;
            // Create quaternion for rotation around Y-axis
            const glm::quat rotation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
            // Apply rotation
            transformComponent.rotation = rotation;
        }

        // scale
        transformComponent.scale = {body.scale, transformComponent.scale.y, body.scale};
    }
}
void bee::PhysicsSystem2D::CheckAndRegisterCollisions()
{
    auto view = m_registry.view<PhysicsBody2DComponent>();
    for (auto [entity1, body1] : view.each())
    {
        for (auto [entity2, body2] : view.each())
        {
            // same entity or entities that have already been checked
            if (entity1 == entity2 || entity1 > entity2)
                continue;

            bool checkAndRegisterCol = true;
            if (m_registry.try_get<AbilityComponent>(entity2))
            {
                if (m_registry.try_get<AbilityComponent>(entity1))
                {
                    checkAndRegisterCol = false;
                }
            }

            if (checkAndRegisterCol)
            {
                CollisionData collision;
                CheckAndRegisterCollision(entity1, body1, entity2, body2, collision);
            }
        }
    }
}

void bee::PhysicsSystem2D::DebugDrawing()
{
#ifdef _DEBUG
    auto view = m_registry.view<PhysicsBody2DComponent, TransformComponent>();
    auto& debugRenderer = Engine.DebugRenderer();
    for (auto [entity, body, transformComponent] : view.each())
    {
        debugRenderer.AddSphere(
            DebugCategory::All, transformComponent.worldTransform[3], body.scale + 0.1f, {1.0f, 0.0f, 1.0f, 1.0f});

        for (auto& col : body.collisions)
        {
            printf(
                "entity%d col - entity1: %d, entity2: %d, normal: (%.2f, %.2f), depth: %.2f, contact point: (%.2f, %.2f)\n",
                to_integral(entity), to_integral(col.entity1), to_integral(col.entity2), col.normal.x, col.normal.y, col.depth,
                col.contactPoint.x, col.contactPoint.y);
        }
    }
#endif
}

bool bee::PhysicsSystem2D::CheckAndRegisterCollision(
    const entt::entity& entity1, PhysicsBody2DComponent& body1, const entt::entity& entity2, PhysicsBody2DComponent& body2,
    CollisionData& collision)
{
    {
        // check for overlap
        glm::vec2 diff = body1.position - body2.position;
        float l2 = glm::length2(diff); // squared length
        float r = body1.scale + body2.scale;
        if (l2 >= r * r)
            return false;

        // compute collision details
        collision.normal = normalize(diff);
        collision.depth = r - sqrt(l2);
        collision.contactPoint = body2.position + collision.normal * body2.scale;

        collision.entity1 = entity1;
        collision.entity2 = entity2;

        body1.AddCollisionData(collision);
        body2.AddCollisionData(
            {collision.entity2, collision.entity1, -collision.normal, collision.depth, collision.contactPoint});

        return true;
    }
}
