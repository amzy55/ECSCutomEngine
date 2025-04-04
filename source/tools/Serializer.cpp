#include "tools/Serializer.h"

#include <fstream>
#include <filesystem>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/optional.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>

#include "core/engine.hpp"
#include "core/input.hpp"
#include "ecs/ECS.h"
#include "ecs/components/PhysicsBody2DComponent.h"
#include "resource_managers/ResourceManager.h"
#include "ecs/components/compact_includes/AllComponents.h"
#include "tools/MainMenuBar.h"

void bee::Serializer::Serialize() const
{
    auto& registry = Engine.ECS().Registry();
    // destroy particles so that they don't get serialized
    auto particleView = registry.view<EmptyParticleComponent>();
    for (auto& particle : particleView)
    {
        registry.destroy(particle);
    }
    //int i = 0;
    //for (auto e : registry.storage<entt::entity>().each())
    //{
    //    i++;
    //}
    //Log::Info("Number of entities: {}", i);

    // destroy all abilities so they don't get serialized
    auto abilityView = registry.view<AbilityComponent>();
    for (auto& ability : abilityView)
    {
        registry.destroy(ability);
    }

    std::ofstream outFile(m_serializationFileName);
    cereal::JSONOutputArchive outArchive(outFile);

    // for every new component you want serialized it has to be added here
    // together with a function to let cereal know how to serialize it.
    // don't forget to add it in Deserialize() as well
    entt::snapshot{registry}
        .get<entt::entity>(outArchive)
        .get<TransformComponent>(outArchive)
        .get<MeshComponent>(outArchive)
        .get<EmitterComponent>(outArchive)
        .get<ParticleConfigComponent>(outArchive)
        .get<HierarchyComponent>(outArchive)
        .get<RootComponent>(outArchive)
        .get<TheoreticalRootComponent>(outArchive)
        .get<NameComponent>(outArchive)
        .get<CameraComponent>(outArchive)
        .get<ScriptComponent>(outArchive)
        .get<PhysicsBody2DComponent>(outArchive)
        .get<PlayerStatsComponent>(outArchive)
        .get<AbilitiesOnPlayerComponent>(outArchive);

    outArchive(cereal::make_nvp("savedAbilities", Engine.ResourceManager().GetAbilityManager().m_abilities));
    outArchive(cereal::make_nvp("viewFlags", Engine.MainMenuBar().GetFlags()));
}

void bee::Serializer::Deserialize() const
{
    if (std::filesystem::exists(m_serializationFileName))
    {
        std::ifstream inFile(m_serializationFileName);
        cereal::JSONInputArchive inArchive(inFile);

        // the registry must be empty when deserializing
        entt::snapshot_loader{Engine.ECS().Registry()}
            .get<entt::entity>(inArchive)
            .get<TransformComponent>(inArchive)
            .get<MeshComponent>(inArchive)
            .get<EmitterComponent>(inArchive)
            .get<ParticleConfigComponent>(inArchive)
            .get<HierarchyComponent>(inArchive)
            .get<RootComponent>(inArchive)
            .get<TheoreticalRootComponent>(inArchive)
            .get<NameComponent>(inArchive)
            .get<CameraComponent>(inArchive)
            .get<ScriptComponent>(inArchive)
            .get<PhysicsBody2DComponent>(inArchive)
            .get<PlayerStatsComponent>(inArchive)
            .get<AbilitiesOnPlayerComponent>(inArchive)
            .orphans(); // it's supposed to release empty entities

        // add all entities that are model roots to the "file paths to entity" map
        const auto view = Engine.ECS().Registry().view<NameComponent, RootComponent>();
        for (const auto [entity, nameComponent] : view.each())
        {
            const std::string filePath = "assets/models/" + nameComponent.name + ".gltf";
            Engine.ResourceManager().GetModelManager().AddEntity(filePath, entity);
        }

        inArchive(Engine.ResourceManager().GetAbilityManager().m_abilities);
        inArchive(Engine.MainMenuBar().m_windowsToDisplayFlags);
    }
}

bool bee::Serializer::IsThereACerealFile() const
{
    if (std::filesystem::exists(m_serializationFileName))
    {
        return true;
    }
    return false;
}

// tell cereal how to serialize non-std types
namespace cereal
{
template <class Archive>
void serialize(Archive& archive, glm::vec2& a)
{
    archive(a.x, a.y);
}

template <class Archive>
void serialize(Archive& archive, glm::vec3& a)
{
    archive(a.x, a.y, a.z);
}

template <class Archive>
void serialize(Archive& archive, glm::vec4& a)
{
    archive(a.x, a.y, a.z, a.w);
}

template <class Archive>
void serialize(Archive& archive, glm::quat& a)
{
    archive(a.w, a.x, a.y, a.z);
}

template <class Archive>
void serialize(Archive& archive, glm::mat4& a)
{
    archive(a[0], a[1], a[2], a[3]);
}

template <class Archive>
void serialize(Archive& archive, xsr::handle& a)
{
    archive(a.id);
}

template <class Archive>
void serialize(Archive& archive, xsr::mesh_handle& a)
{
    archive(a.id);
}

template <class Archive>
void serialize(Archive& archive, xsr::texture_handle& a)
{
    archive(a.id);
}

template <class Archive>
void serialize(Archive& archive, bee::SimpleMesh& a)
{
    archive(a.handle);
}

template <class Archive>
void serialize(Archive& archive, bee::SimpleTexture& a)
{
    archive(a.handle);
}

template <>
struct LoadAndConstruct<bee::SimpleMesh>
{
    template <class Archive>
    static void load_and_construct(Archive& archive, cereal::construct<bee::SimpleMesh>& construct)
    {
        xsr::mesh_handle handle;
        archive(handle);
        construct(handle);
    }
};

template <>
struct LoadAndConstruct<bee::SimpleTexture>
{
    template <class Archive>
    static void load_and_construct(Archive& archive, cereal::construct<bee::SimpleTexture>& construct)
    {
        xsr::texture_handle handle;
        archive(handle);
        construct(handle);
    }
};

template <class Archive>
void serialize(Archive& archive, bee::TransformComponent& a)
{
    archive(a.pos, a.rotation, a.scale, a.worldTransform);
}

template <class Archive>
void serialize(Archive& archive, bee::EmitterComponent& a)
{
    archive(a.particlesPerSecond, a.endless, a.lifeSpan, a.currentLife);
}

template <class Archive>
void serialize(Archive& archive, bee::ParticleConfigComponent& a)
{
    archive(a.gravity, a.dir, a.speed, a.colorBegin, a.colorEnd, a.scaleBegin, a.scaleEnd, a.lifeSpan);
}

template <class Archive>
void serialize(Archive& archive, bee::CameraComponent& a)
{
    archive(a.view, a.projection, a.speed);
}

template <class Archive>
void serialize(Archive& archive, bee::HierarchyComponent& a)
{
    archive(a.parent, a.children, a.isSelected);
}

template <class Archive>
void serialize(Archive& archive, bee::NameComponent& a)
{
    archive(a.name);
}

template <class Archive>
void serialize(Archive& archive, bee::ScriptComponent& a)
{
    archive(a.scripts);
}

template <class Archive>
void serialize(Archive& archive, bee::MeshComponent& a)
{
    archive(a.mesh, a.texture, a.mulColor, a.addColor);
}

template <class Archive>
void serialize(Archive& archive, bee::RootComponent& a)
{
}

template <class Archive>
void serialize(Archive& archive, bee::TheoreticalRootComponent& a)
{
}

template <class Archive>
void serialize(Archive& archive, bee::PhysicsBody2DComponent& a)
{
    archive(a.position, a.velocity, a.scale);
}

template <class Archive>
void save(Archive& archive, const bee::PlayerStatsComponent& a)
{
    archive(
        a.id, a.teamId, a.baseHealth, a.baseAmmo, a.baseReloadSpeed, a.baseMovementSpeed, a.baseDealtDamageModifier,
        a.baseReceivedDamageReduction);
}

template <class Archive>
void load(Archive& archive, bee::PlayerStatsComponent& a)
{
    archive(
        a.id, a.teamId, a.baseHealth, a.baseAmmo, a.baseReloadSpeed, a.baseMovementSpeed, a.baseDealtDamageModifier,
        a.baseReceivedDamageReduction);
    a.currentHealth = a.baseHealth;
    a.currentAmmo = a.baseAmmo;
    a.currentReloadSpeed = a.baseReloadSpeed;
    a.currentMovementSpeed = a.baseMovementSpeed;
    a.currentDealtDamageModifier = a.baseDealtDamageModifier;
    a.currentReceivedDamageReduction = a.baseReceivedDamageReduction;
}

template <class Archive>
void serialize(Archive& archive, bee::AbilitiesOnPlayerComponent& a)
{
    archive(a.abilityNamesToInputsMap, a.maxNumberOfAbilities);
}

template <class Archive>
void save(Archive& archive, const bee::AbilitySettings& a)
{
    archive(
        static_cast<std::underlying_type_t<bee::Target>>(a.targetTeam), a.aoeRadius, a.numberOfProjectiles, a.projectileRange,
        a.projectileRadius, a.projectileSpeed, static_cast<std::underlying_type_t<bee::ProjectileShootType>>(a.shootType),
        a.spaceBetweenProjectiles, a.coneAngle, static_cast<std::underlying_type_t<bee::CastType>>(a.castType),
        static_cast<std::underlying_type_t<bee::ApplyType>>(a.applyType),
        static_cast<std::underlying_type_t<bee::InstantType>>(a.instantType), a.dashSpeed,
        static_cast<std::underlying_type_t<bee::StatusEffectType>>(a.statusEffectType),
        static_cast<std::underlying_type_t<bee::Stat>>(a.statAffected), a.statAffectAmount,
        static_cast<std::underlying_type_t<bee::IncreaseOrDecrease>>(a.increaseOrDecrease), a.clampToMax,
        static_cast<std::underlying_type_t<bee::AppliedAs>>(a.appliedAs), a.durationAmount, a.numberOfTicks);
}

template <class Archive>
void load(Archive& archive, bee::AbilitySettings& a)
{
    std::underlying_type_t<bee::Target> targetTeam;
    std::underlying_type_t<bee::ProjectileShootType> shootType;
    std::underlying_type_t<bee::CastType> castType;
    std::underlying_type_t<bee::ApplyType> applyType;
    std::underlying_type_t<bee::InstantType> instantType;
    std::underlying_type_t<bee::StatusEffectType> statusEffect;
    std::underlying_type_t<bee::Stat> statAffected;
    std::underlying_type_t<bee::IncreaseOrDecrease> increaseOrDecrease;
    std::underlying_type_t<bee::AppliedAs> appliedAs;

    archive(
        targetTeam, a.aoeRadius, a.numberOfProjectiles, a.projectileRange, a.projectileRadius, a.projectileSpeed, shootType,
        a.spaceBetweenProjectiles, a.coneAngle, castType, applyType, instantType, a.dashSpeed, statusEffect, statAffected,
        a.statAffectAmount, increaseOrDecrease, a.clampToMax, appliedAs, a.durationAmount, a.numberOfTicks);

    a.targetTeam = static_cast<bee::Target>(targetTeam);
    a.shootType = static_cast<bee::ProjectileShootType>(shootType);
    a.castType = static_cast<bee::CastType>(castType);
    a.applyType = static_cast<bee::ApplyType>(applyType);
    a.instantType = static_cast<bee::InstantType>(instantType);
    a.statusEffectType = static_cast<bee::StatusEffectType>(statusEffect);
    a.statAffected = static_cast<bee::Stat>(statAffected);
    a.increaseOrDecrease = static_cast<bee::IncreaseOrDecrease>(increaseOrDecrease);
    a.appliedAs = static_cast<bee::AppliedAs>(appliedAs);
}

} //namespace cereal
