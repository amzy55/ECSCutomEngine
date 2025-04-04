#pragma once
#include "core/engine.hpp"
#include "ecs/ECS.h"
#include "ecs/components/PlayerStatsComponents.h"
#include "Jolt/Physics/Collision/ContactListener.h"

class SceneContactListener : public JPH::ContactListener
{
public:
    // cannot use JPH::BodyID key because of hash redefinition
    std::unordered_map<std::uint32_t, entt::entity> m_bodyIDToEntity;

    //virtual JPH::ValidateResult OnContactValidate(
    //    const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult
    //    &inCollisionResult) override
    //{
    //    bee::Log::Info("Contact validate callback");

    //    // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
    //    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    //}

    virtual void OnContactAdded(
        const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override
    {
        entt::entity player0Ent;
        entt::entity player1Ent;
        auto& registry = bee::Engine.ECS().Registry();
        auto playerView = registry.view<bee::PlayerStatsComponent>();
        for (auto [entity, playerComponent] : playerView.each())
        {
            if (playerComponent.id == 0)
                player0Ent = entity;
            else if (playerComponent.id == 1)
                player1Ent = entity;
        }
        if (m_bodyIDToEntity[inBody1.GetID().GetIndex()] == player0Ent &&
            m_bodyIDToEntity[inBody2.GetID().GetIndex()] == player1Ent)
        {
            auto& health = registry.get<bee::PlayerStatsComponent>(player0Ent).currentHealth;
            health = std::max(0.0f, health - 15.f);
            //if (are_floats_equal(health, 0.0f))
            //{
            //    auto& meshComponentColor = registry.get<bee::MeshComponent>(player0Ent).mulColor;
            //    meshComponentColor = {1.0f, 0.0f, 0.0f, 1.0f};
            //}
        }
    }

    //virtual void OnContactPersisted(
    //    const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings
    //    &ioSettings) override
    //{
    //    bee::Log::Info("A contact was persisted");
    //}

    //virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
    //{
    //    bee::Log::Info("A contact was removed");
    //}
};
