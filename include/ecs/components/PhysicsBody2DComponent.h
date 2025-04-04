#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <entt/entity/entity.hpp>

namespace bee
{

/**
 * \brief Stores the details of a single collision.
 */
struct CollisionData
{
    /**
     * \brief The ID of the first entity involved in the collision.
     */
    entt::entity entity1;

    /**
     * \brief The ID of the second entity involved in the collision.
     */
    entt::entity entity2;

    /**
     * \brief The normal vector on the point of contact, pointing away from entity2's physics body.
     */
    glm::vec2 normal;
    
    /**
     * \brief The penetration depth of the two physics bodies
     * (before they were displaced to resolve overlap).
     */
    float depth;

    /**
     * \brief The approximate point of contact of the collision, in world coordinates.
     */
    glm::vec2 contactPoint;
};

/**
 * \brief Physics body for 2D physics system with a disk collider.
 */
struct PhysicsBody2DComponent
{
    PhysicsBody2DComponent(const glm::vec2 pos = {0.f, 0.f}, const float radius = 1.f, const glm::vec2 vel = {0.f, 0.f})
        : position(pos), velocity(vel), scale(radius)
    {}

    glm::vec2 position = glm::vec2(0.f);
    glm::vec2 velocity = glm::vec2(0.f);
    float scale = 1.f; // radius of the disk

    std::vector<CollisionData> collisions = {};
    void AddCollisionData(const CollisionData& data) { collisions.push_back(data); }
    void ClearCollisionData() { collisions.clear(); }

    // Clear collision data and update position based on velocity
    void Update(const float dt)
    {
        ClearCollisionData();
        position += velocity * dt;
    }
};

}
