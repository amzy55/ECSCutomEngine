#include "ecs/systems/AbilitySystem.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/engine.hpp"
#include "core/input.hpp"
#include "ecs/ECS.h"
#include "resource_managers/ResourceManager.h"

#include "ecs/components/PlayerStatsComponents.h"
#include "ecs/components/AbilityComponent.h"
#include "ecs/components/PhysicsBody2DComponent.h"
#include "ecs/components/compact_includes/HierarchyIncludeComponents.h"
#include "tools/MainMenuBar.h"
#include "tools/tools.hpp"

constexpr float pi = 3.1415927f;

bee::AbilitySystem::AbilitySystem(entt::registry& registry) : BaseSystem(registry)
{
    m_statColors = {
        {{Stat::Health, IncreaseOrDecrease::Increase}, {0.f, 1.f, 0.f, 1.0f}},
        {{Stat::Health, IncreaseOrDecrease::Decrease}, {1.f, 0.f, 0.f, 1.0f}},
        {{Stat::MovementSpeed, IncreaseOrDecrease::Increase}, {1.f, 0.5f, 0.f, 1.0f}},
        {{Stat::MovementSpeed, IncreaseOrDecrease::Decrease}, {0.25f, 0.25f, 1.f, 1.0f}},
        {{Stat::DealtDamageModifier, IncreaseOrDecrease::Increase}, {0.75f, 0.25f, 0.75f, 1.0f}},
        {{Stat::ReceivedDamageReduction, IncreaseOrDecrease::Increase}, {1.f, 1.f, 0.25f, 1.0f}},
    };
    m_statNamesForVisualEffectsEditWindow = {
        "Health Increase",         "Health Decrease",       "Movement Speed Increase",
        "Movement Speed Decrease", "Dealt Damage Increase", "Received Damage Reduction Increase",
    };
}

void bee::AbilitySystem::Update(const float& dt)
{
    UpdateAmmoAndDisplayVisualEffects(dt);
    UpdateAbilities(dt);
    CheckInputToCreateAbilities();
    DisplayVisualEffectsEditor();
}

void bee::AbilitySystem::UpdateAmmoAndDisplayVisualEffects(const float dt)
{
    auto playerView = m_registry.view<PlayerStatsComponent, MeshComponent>();
    for (auto [entity, playerStats, mesh] : playerView.each())
    {
        // update ammo
        if (playerStats.currentAmmo < playerStats.baseAmmo)
        {
            playerStats.currentAmmo += playerStats.currentReloadSpeed * dt;
        }

        // visual effects
        for (auto visualEffectIter = playerStats.visualEffects.begin(); visualEffectIter != playerStats.visualEffects.end();)
        {
            mesh.mulColor = visualEffectIter->color;
            visualEffectIter->currentDuration -= dt;
            if (visualEffectIter->currentDuration <= 0.f)
            {
                mesh.mulColor = m_teamColors[playerStats.teamId];
                visualEffectIter = playerStats.visualEffects.erase(visualEffectIter);
            }
            else
            {
                ++visualEffectIter;
            }
        }
    }
}

void bee::AbilitySystem::UpdateAbilities(const float dt)
{
    auto abilityView = m_registry.view<AbilityComponent>();
    for (auto [ability, abilityComponent] : abilityView.each())
    {
        auto& castByPlayerStats = m_registry.get<PlayerStatsComponent>(abilityComponent.castByPlayer);
        auto& abilitySettings = abilityComponent.settings;
        const bool targetSelf = abilitySettings.targetTeam == Target::Self;

        // dash
        if (targetSelf && abilitySettings.applyType == ApplyType::Instant && abilitySettings.instantType == InstantType::Dash)
        {
            abilityComponent.currentDurationAmount -= dt;
            if (abilityComponent.currentDurationAmount <= 0.0f)
            {
                auto& playerPhysicsBody = m_registry.get<PhysicsBody2DComponent>(abilityComponent.castByPlayer);
                playerPhysicsBody.velocity = glm::vec2(0.f);
                castByPlayerStats.state &= ~PlayerStatsComponent::State::Dashing; // dashing off
                m_registry.destroy(ability);
            }
        }
        else
        {
            // check for hit to update and apply durational effects
            auto* playerHit = m_registry.try_get<PlayerStatsComponent>(abilityComponent.hitPlayer);
            if (playerHit != nullptr)
            {
                if (targetSelf || (abilitySettings.castType == CastType::Projectile && abilityComponent.dead ||
                                   abilitySettings.castType == CastType::RadiusAroundPlayer && !abilityComponent.dead))
                {
                    auto& hitPlayerStats = *playerHit;
                    abilityComponent.currentDurationAmount -= dt;
                    if (abilityComponent.currentDurationAmount <= 0.0f)
                    {
                        switch (abilitySettings.applyType)
                        {
                            case ApplyType::Instant:
                            {
                                break;
                            }
                            case ApplyType::EffectOverTime:
                            {
                                abilityComponent.currentDurationAmount = abilitySettings.durationAmount;
                                abilityComponent.currentNumberOfTicks--;
                                ApplyAmount(abilityComponent.settings, hitPlayerStats, castByPlayerStats, false);

                                if (abilityComponent.currentNumberOfTicks <= 0)
                                {
                                    m_registry.destroy(ability);
                                }
                                break;
                            }
                            case ApplyType::BuffDebuff:
                            {
                                ApplyAmount(abilityComponent.settings, hitPlayerStats, castByPlayerStats, true);
                                m_registry.destroy(ability);
                                break;
                            }
                            case ApplyType::StatusEffect:
                            {
                                hitPlayerStats.state &= ~(1 << static_cast<unsigned>(abilitySettings.statusEffectType));
                                m_registry.destroy(ability);
                                break;
                            }
                        }
                    }
                }
            }
        }

        // cast abilities
        if (!targetSelf)
        {
            // check for hit
            const auto tryGetPhysicsBody = m_registry.try_get<PhysicsBody2DComponent>(ability);
            bool abilityDeadCondition = abilitySettings.castType == CastType::Projectile ? !abilityComponent.dead : true;
            if (tryGetPhysicsBody != nullptr && abilityDeadCondition)
            {
                auto& physicsBody = *tryGetPhysicsBody;
                for (const auto& collision : physicsBody.collisions)
                {
                    auto checkIfPlayerHit = m_registry.try_get<PlayerStatsComponent>(collision.entity2);
                    if (checkIfPlayerHit)
                    {
                        auto& hitPlayerStats = *checkIfPlayerHit;
                        if (castByPlayerStats.teamId != hitPlayerStats.teamId &&
                                abilitySettings.targetTeam == Target::Hostile ||
                            castByPlayerStats.teamId == hitPlayerStats.teamId &&
                                (abilitySettings.targetTeam == Target::Friendly && castByPlayerStats.id != hitPlayerStats.id ||
                                 abilitySettings.targetTeam == Target::SelfAndFriendly))
                        {
                            switch (abilitySettings.castType)
                            {
                                case CastType::Projectile:
                                {
                                    ApplyAmount(abilitySettings, hitPlayerStats, castByPlayerStats, false);
                                    switch (abilitySettings.applyType)
                                    {
                                        case ApplyType::Instant:
                                        {
                                            DestroyOrEraseComponentsBasedOnFirstProjectileInLine(ability, abilityComponent);
                                            break;
                                        }
                                        case ApplyType::EffectOverTime:
                                        case ApplyType::BuffDebuff:
                                        case ApplyType::StatusEffect:
                                        {
                                            abilityComponent.hitPlayer = collision.entity2;
                                            RemoveFromHierarchy(ability);
                                            m_registry.erase<HierarchyComponent, MeshComponent, TransformComponent>(ability);
                                            abilityComponent.dead = true;
                                            if (abilitySettings.applyType == ApplyType::EffectOverTime)
                                            {
                                                abilityComponent.currentNumberOfTicks--;
                                            }
                                            if (abilitySettings.applyType == ApplyType::StatusEffect)
                                            {
                                                ApplyStatusEffect(abilitySettings, hitPlayerStats);
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                }
                                case CastType::RadiusAroundPlayer:
                                {
                                    ApplyAmount(abilitySettings, hitPlayerStats, castByPlayerStats, false);
                                    switch (abilitySettings.applyType)
                                    {
                                        case ApplyType::Instant:
                                        {
                                            break;
                                        }
                                        case ApplyType::EffectOverTime:
                                        case ApplyType::BuffDebuff:
                                        case ApplyType::StatusEffect:
                                        {
                                            auto durationalAbility = m_registry.create();
                                            auto& durationalAbilityComponent =
                                                m_registry.emplace<AbilityComponent>(durationalAbility, abilityComponent);
                                            durationalAbilityComponent.hitPlayer = collision.entity2;
                                            durationalAbilityComponent.dead = false;
                                            if (abilitySettings.applyType == ApplyType::EffectOverTime)
                                            {
                                                durationalAbilityComponent.currentNumberOfTicks--;
                                            }
                                            if (durationalAbilityComponent.settings.applyType == ApplyType::StatusEffect)
                                            {
                                                ApplyStatusEffect(durationalAbilityComponent.settings, hitPlayerStats);
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
                if (abilitySettings.castType == CastType::RadiusAroundPlayer)
                {
                    // if an aoe ability has hit at least one player delete the physics body
                    // so it cannot register more collisions
                    m_registry.erase<PhysicsBody2DComponent>(ability);
                }
            }
            // actions that need to be performed regardless if the ability has hit a player or not
            switch (abilitySettings.castType)
            {
                case CastType::Projectile:
                {
                    // update projectile range
                    const bool maxRangeReached = abilityComponent.currentProjectileRange >= abilitySettings.projectileRange;
                    if (maxRangeReached && !abilityComponent.dead)
                    {
                        DestroyOrEraseComponentsBasedOnFirstProjectileInLine(ability, abilityComponent);
                    }
                    abilityComponent.currentProjectileRange += abilitySettings.projectileSpeed * dt;

                    const bool firstProjectileInLine = abilitySettings.shootType == ProjectileShootType::Line &&
                                                       abilityComponent.firstProjectileInLine == true;
                    if (firstProjectileInLine)
                    {
                        ProjectilesInLineLogic(ability, abilityComponent);
                    }
                    break;
                }
                case CastType::RadiusAroundPlayer:
                {
                    if (abilityComponent.dead == true)
                    {
                        abilityComponent.currentAoeDuration -= dt;
                        if (abilityComponent.currentAoeDuration <= 0.f)
                        {
                            RemoveFromHierarchy(ability);
                            m_registry.destroy(ability);
                        }
                    }
                    break;
                }
            }
        }
    }
}

void bee::AbilitySystem::CheckInputToCreateAbilities()
{
    auto abilitiesOnPlayerView = m_registry.view<AbilitiesOnPlayerComponent, PlayerStatsComponent>();
    for (auto [entity, ability, playerStats] : abilitiesOnPlayerView.each())
    {
        if (playerStats.state & PlayerStatsComponent::State::Dashing ||
            playerStats.state & PlayerStatsComponent::State::Silenced ||
            playerStats.state & PlayerStatsComponent::State::Stunned)
            continue;

        for (auto& [abilityName, inputBindings] : ability.abilityNamesToInputsMap)
        {
            if (inputBindings.first != std::nullopt)
            {
                if (Engine.Input().GetKeyboardKeyOnce(inputBindings.first.value()))
                {
                    CreateAbility(abilityName, entity);
                }
            }
            if (inputBindings.second != std::nullopt)
            {
                if (Engine.Input().GetGamepadButtonOnce(playerStats.id, inputBindings.second.value()))
                {
                    CreateAbility(abilityName, entity);
                }
            }
        }
    }
}

void bee::AbilitySystem::DisplayVisualEffectsEditor()
{
    if (!Engine.MainMenuBar().IsFlagOn(WindowsToDisplay::VisualEffectsEditor))
        return;

    ImGui::Begin("Visual Effects Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    int i = 0;
    for (auto& [stat, color] : m_statColors)
    {
        ImGui::ColorEdit4(m_statNamesForVisualEffectsEditWindow[i++].c_str(), value_ptr(color));
    }
    ImGui::ColorEdit4("Stun | Root | Silence", value_ptr(m_stunRootSilenceColor));
    ImGui::ColorEdit4("Invulnerability", value_ptr(m_invincibilityColor));
    ImGui::ColorEdit4("All Other Positive Effects", value_ptr(m_statDefaultColors[0]));
    ImGui::ColorEdit4("All Other Negative Effects", value_ptr(m_statDefaultColors[1]));
    ImGui::End();
}

void bee::AbilitySystem::CreateAbility(const std::string& abilityName, entt::entity castByPlayer)
{
    auto& playerStats = m_registry.get<PlayerStatsComponent>(castByPlayer);

    if (playerStats.currentAmmo < 1.f)
        return; // not enough ammo

    // decrease ammo
    playerStats.currentAmmo -= 1.0f;

    // create ability
    auto ability = m_registry.create();
    auto& abilityComponent = m_registry.emplace<AbilityComponent>(
        ability, castByPlayer,
        Engine.ResourceManager().GetAbilityManager().Get(abilityName).value()); // NOLINT(bugprone-unchecked-optional-access)

    auto& abilitySettings = abilityComponent.settings;

    if (abilitySettings.targetTeam == Target::Self)
    {
        switch (abilitySettings.applyType)
        {
            case ApplyType::Instant:
            {
                switch (abilitySettings.instantType)
                {
                    case InstantType::Effect:
                    {
                        ApplyAmount(abilitySettings, playerStats, playerStats, false);
                        m_registry.destroy(ability);
                        break;
                    }
                    case InstantType::Dash:
                    {
                        auto& playerPhysicsBody = m_registry.get<PhysicsBody2DComponent>(abilityComponent.castByPlayer);
                        const auto& playerTransform = m_registry.get<TransformComponent>(abilityComponent.castByPlayer);
                        const glm::vec2 direction2D = GetXZDirectionFromQuaternion(playerTransform.rotation);
                        playerPhysicsBody.velocity = direction2D * abilitySettings.dashSpeed;
                        playerStats.state = PlayerStatsComponent::State::Dashing;
                        break;
                    }
                }
                break;
            }
            case ApplyType::EffectOverTime:
            case ApplyType::BuffDebuff:
            {
                ApplyAmount(abilitySettings, playerStats, playerStats, false);
                if (abilitySettings.applyType == ApplyType::EffectOverTime)
                {
                    abilityComponent.currentNumberOfTicks--;
                }
                break;
            }
            case ApplyType::StatusEffect:
            {
                ApplyStatusEffect(abilitySettings, playerStats);
                break;
            }
        }
        abilityComponent.hitPlayer = castByPlayer;
    }
    else // all other options other than self
    {
        switch (abilitySettings.castType)
        {
            case CastType::Projectile:
            {
                const auto mesh =
                    Engine.ResourceManager().GetMeshManager().Get(std::make_pair("assets/models/mycube.gltf", 0)).value();
                const auto texture = Engine.ResourceManager().GetTextureManager().Get("white").value();
                MeshComponent meshComponent = {mesh, texture, m_teamColors[playerStats.teamId]};
                auto abilityPhysicsBody = PhysicsBody2DComponent();
                const auto& playerPhysicsBody = m_registry.get<PhysicsBody2DComponent>(abilityComponent.castByPlayer);
                const auto& playerTransform = m_registry.get<TransformComponent>(abilityComponent.castByPlayer);
                const glm::vec2 direction2D = GetXZDirectionFromQuaternion(playerTransform.rotation);
                abilityPhysicsBody.velocity = direction2D * abilitySettings.projectileSpeed;
                abilityPhysicsBody.scale = abilitySettings.projectileRadius;
                abilityPhysicsBody.position = playerPhysicsBody.position;
                // special case where the cone is 360 degrees so the projectiles should not spawn in front of the player
                if (!(are_floats_equal(abilitySettings.coneAngle, 2.f * pi) &&
                      abilitySettings.shootType == ProjectileShootType::Cone))
                {
                    const glm::vec2 spawnOffset = direction2D * (playerPhysicsBody.scale + abilityPhysicsBody.scale);
                    abilityPhysicsBody.position += spawnOffset;
                }
                auto abilityTransform = TransformComponent();
                abilityTransform.pos.y = playerTransform.pos.y;
                abilityTransform.scale.y = abilitySettings.projectileRadius;

                if (abilitySettings.numberOfProjectiles == 1)
                {
                    auto projectile = CreateProjectile(abilityComponent, meshComponent, abilityPhysicsBody, abilityTransform);
                    AddToHierarchy(projectile, abilityName);
                }
                else
                {
                    switch (abilitySettings.shootType)
                    {
                        case ProjectileShootType::Cone:
                        {
                            const float angleBetweenProjectiles =
                                abilitySettings.coneAngle / static_cast<float>(abilitySettings.numberOfProjectiles - 1);
                            const glm::vec2 normalizedDir = normalize(abilityPhysicsBody.velocity);
                            const float directionInRadians = atan2(normalizedDir.y, normalizedDir.x);
                            const float firstProjectileAngle = directionInRadians - abilitySettings.coneAngle / 2.f;

                            for (int i = 0; i < abilitySettings.numberOfProjectiles; i++)
                            {
                                auto projectile =
                                    CreateProjectile(abilityComponent, meshComponent, abilityPhysicsBody, abilityTransform);

                                // calculate direction
                                auto& physicsBody = m_registry.get<PhysicsBody2DComponent>(projectile);
                                const auto projectileAngle =
                                    firstProjectileAngle + static_cast<float>(i) * angleBetweenProjectiles;
                                physicsBody.velocity =
                                    glm::vec2(cos(projectileAngle), sin(projectileAngle)) * length(physicsBody.velocity);

                                AddToHierarchy(projectile, abilityName + std::to_string(i));
                            }
                            m_registry.destroy(ability);
                            break;
                        }
                        case ProjectileShootType::Line:
                        {
                            auto projectile =
                                CreateProjectile(abilityComponent, meshComponent, abilityPhysicsBody, abilityTransform);
                            AddToHierarchy(projectile, abilityName + std::to_string(1));
                            auto& projectileAbilityComponent = m_registry.get<AbilityComponent>(projectile);
                            projectileAbilityComponent.firstProjectileInLine = true;
                            m_registry.destroy(ability);
                            break;
                        }
                    }
                }
                break;
            }
            case CastType::RadiusAroundPlayer:
            {
                auto& meshComponent = m_registry.emplace<MeshComponent>(ability);
                meshComponent.mesh =
                    Engine.ResourceManager().GetMeshManager().Get(std::make_pair("assets/models/cylinder.gltf", 0)).value();
                meshComponent.texture = Engine.ResourceManager().GetTextureManager().Get("white").value();
                meshComponent.mulColor = m_teamColors[playerStats.teamId];

                auto& physicsBody = m_registry.emplace<PhysicsBody2DComponent>(ability);
                const auto& playerPhysicsBody = m_registry.get<PhysicsBody2DComponent>(abilityComponent.castByPlayer);
                physicsBody.position = playerPhysicsBody.position;
                physicsBody.scale = abilitySettings.aoeRadius;

                auto& transformComponent = m_registry.emplace<TransformComponent>(ability);
                transformComponent.scale.y = 0.1f;

                AddToHierarchy(ability, abilityName);
                
                abilityComponent.dead = true;
                break;
            }
        }
    }
}

std::pair<float&, float&> bee::AbilitySystem::GetStat(const Stat stat, PlayerStatsComponent& playerStats)
{
    switch (stat)
    {
        case Stat::Health:
            return {playerStats.currentHealth, playerStats.baseHealth};
        case Stat::Ammo:
            return {playerStats.currentAmmo, playerStats.baseAmmo};
        case Stat::ReloadSpeed:
            return {playerStats.currentReloadSpeed, playerStats.baseReloadSpeed};
        case Stat::MovementSpeed:
            return {playerStats.currentMovementSpeed, playerStats.baseMovementSpeed};
        case Stat::DealtDamageModifier:
            return {playerStats.currentDealtDamageModifier, playerStats.baseDealtDamageModifier};
        case Stat::ReceivedDamageReduction:
            return {playerStats.currentReceivedDamageReduction, playerStats.baseReceivedDamageReduction};
    }

    // added this just because the compiler was complaining that somehow
    // not all control paths return a value
    return {playerStats.currentHealth, playerStats.baseHealth};
}

void bee::AbilitySystem::ApplyAmount(
    const AbilitySettings& ability, PlayerStatsComponent& hitPlayerStats, const PlayerStatsComponent& castByPlayerStats,
    const bool revert)
{
    if (hitPlayerStats.state & PlayerStatsComponent::State::Invincible &&
        ability.increaseOrDecrease == IncreaseOrDecrease::Decrease)
    {
        return; // do not apply any negative effects
    }

    auto [current, base] = GetStat(ability.statAffected, hitPlayerStats);
    float amount = ability.statAffectAmount;

    if (ability.appliedAs == AppliedAs::Percentage)
    {
        if (ability.statAffected != Stat::DealtDamageModifier && ability.statAffected != Stat::ReceivedDamageReduction)
            amount = base * (amount / 100.f);
        else
            amount = amount / 100.f;
    }

    if (ability.statAffected == Stat::Health)
    {
        const float damageModifier =
            castByPlayerStats.currentDealtDamageModifier - hitPlayerStats.currentReceivedDamageReduction;
        amount += amount * damageModifier;
    }

    if (ability.increaseOrDecrease == IncreaseOrDecrease::Decrease)
    {
        amount = -amount;
    }

    if (revert)
    {
        amount = -amount;
    }
    else if (ability.applyType != ApplyType::StatusEffect)
    {
        AddVisualEffectForApplyAmount(ability, hitPlayerStats);
    }

    // apply
    current += amount;
    current = std::max(current, 0.0f);
    if (ability.clampToMax)
        current = std::min(current, base);
}

void bee::AbilitySystem::ApplyStatusEffect(const AbilitySettings& ability, PlayerStatsComponent& playerStats)
{
    if (playerStats.state & PlayerStatsComponent::State::Invincible &&
        ability.statusEffectType != StatusEffectType::Invincibility)
    {
        return; // do not apply any negative effects
    }

    AddVisualEffectForStatusEffect(ability, playerStats);
    playerStats.state |= 1 << static_cast<unsigned>(ability.statusEffectType);
}

void bee::AbilitySystem::AddToHierarchy(entt::entity entity, const std::string& name)
{
    auto theoreticalRootView = m_registry.view<TheoreticalRootComponent>();
    auto& rootHierarchyComponent = m_registry.get<HierarchyComponent>(theoreticalRootView.front());
    m_registry.emplace<RootComponent>(entity);
    auto& hierarchyComponent = m_registry.emplace<HierarchyComponent>(entity);
    hierarchyComponent.parent = theoreticalRootView.front();
    rootHierarchyComponent.children.push_back(entity);
    m_registry.emplace<NameComponent>(entity, name);
}

void bee::AbilitySystem::RemoveFromHierarchy(entt::entity entity)
{
    auto theoreticalRootView = m_registry.view<TheoreticalRootComponent>();
    auto& rootHierarchyComponent = m_registry.get<HierarchyComponent>(theoreticalRootView.front());
    for (auto it = rootHierarchyComponent.children.begin(); it != rootHierarchyComponent.children.end(); ++it)
    {
        if (*it == entity)
        {
            rootHierarchyComponent.children.erase(it);
            break;
        }
    }
}

entt::entity bee::AbilitySystem::CreateProjectile(const AbilityComponent& abilityComponent, const MeshComponent& meshComponent,
    const PhysicsBody2DComponent& physicsBody, const TransformComponent& transformComponent)
{
    auto projectile = m_registry.create();
    m_registry.emplace<AbilityComponent>(projectile, abilityComponent);
    m_registry.emplace<MeshComponent>(projectile, meshComponent);
    m_registry.emplace<PhysicsBody2DComponent>(projectile, physicsBody);
    m_registry.emplace<TransformComponent>(projectile, transformComponent);

    return projectile;
}

void bee::AbilitySystem::DestroyOrEraseComponentsBasedOnFirstProjectileInLine(
    entt::entity ability, AbilityComponent& abilityComponent)
{
    if (!abilityComponent.firstProjectileInLine)
    {
        RemoveFromHierarchy(ability);
        m_registry.destroy(ability);
    }
    else if (abilityComponent.firstProjectileInLine)
    {
        RemoveFromHierarchy(ability);
        // do not destroy the first projectile until
        // all the other projectiles in the line have been spawned
        m_registry.erase<HierarchyComponent, MeshComponent, TransformComponent>(ability);
        abilityComponent.dead = true;
    }
}

void bee::AbilitySystem::ProjectilesInLineLogic(entt::entity ability, AbilityComponent& abilityComponent)
{
    auto& abilitySettings = abilityComponent.settings;
    if (static_cast<int>(
            abilityComponent.currentProjectileRange /
            (abilitySettings.projectileRadius * 2.f + abilitySettings.spaceBetweenProjectiles)) >=
            abilityComponent.currentNumberOfProjectiles &&
        abilityComponent.currentNumberOfProjectiles < abilitySettings.numberOfProjectiles)
    {
        abilityComponent.currentNumberOfProjectiles++;

        const auto mesh = Engine.ResourceManager()
                              .GetMeshManager()
                              .Get(std::make_pair("assets/models/mycube.gltf", 0))
                              .value(); // NOLINT(bugprone-unchecked-optional-access)
        const auto texture = Engine.ResourceManager()
                                 .GetTextureManager()
                                 .Get("white")
                                 .value(); // NOLINT(bugprone-unchecked-optional-access)
        const auto& playerStats = m_registry.get<PlayerStatsComponent>(abilityComponent.castByPlayer);
        MeshComponent newProjectileMeshComponent = {mesh, texture, m_teamColors[playerStats.teamId]};
        auto newProjectilePhysicsBody = PhysicsBody2DComponent();
        const auto& playerPhysicsBody = m_registry.get<PhysicsBody2DComponent>(abilityComponent.castByPlayer);
        const auto& playerTransform = m_registry.get<TransformComponent>(abilityComponent.castByPlayer);
        const glm::vec2 direction2D = GetXZDirectionFromQuaternion(playerTransform.rotation);
        newProjectilePhysicsBody.velocity = direction2D * abilitySettings.projectileSpeed;
        newProjectilePhysicsBody.scale = abilitySettings.projectileRadius;
        const glm::vec2 spawnOffset = direction2D * (playerPhysicsBody.scale + newProjectilePhysicsBody.scale);
        newProjectilePhysicsBody.position = playerPhysicsBody.position + spawnOffset;
        auto newProjectileTransform = TransformComponent();
        newProjectileTransform.pos.y = playerTransform.pos.y;
        newProjectileTransform.scale.y = abilitySettings.projectileRadius;

        auto newProjectile =
            CreateProjectile(abilityComponent, newProjectileMeshComponent, newProjectilePhysicsBody, newProjectileTransform);
        auto& newProjectileAbilityComponent = m_registry.get<AbilityComponent>(newProjectile);
        newProjectileAbilityComponent.firstProjectileInLine = false;
        newProjectileAbilityComponent.dead = false;
        newProjectileAbilityComponent.currentProjectileRange = 0.0f;
        newProjectileAbilityComponent.currentDurationAmount = newProjectileAbilityComponent.settings.durationAmount;
        auto name = m_registry.get<NameComponent>(ability).name;
        name = StringRemove(name, "1");
        AddToHierarchy(newProjectile, name + std::to_string(abilityComponent.currentNumberOfProjectiles));
    }
    if (abilityComponent.currentNumberOfProjectiles >= abilitySettings.numberOfProjectiles &&
        abilityComponent.currentProjectileRange >= abilitySettings.projectileRange)
    {
        m_registry.destroy(ability);
        if (!abilityComponent.dead)
            RemoveFromHierarchy(ability);
    }
}

void bee::AbilitySystem::AddVisualEffectForApplyAmount(const AbilitySettings& ability, PlayerStatsComponent& playerStats)
{
    glm::vec4 color;
    if (const auto it = m_statColors.find(std::make_pair(ability.statAffected, ability.increaseOrDecrease));
        it != m_statColors.end())
    {
        color = it->second;
    }
    else
    {
        color = m_statDefaultColors[static_cast<unsigned>(ability.increaseOrDecrease)];
    }

    if (ability.applyType == ApplyType::Instant || ability.applyType == ApplyType::EffectOverTime)
    {
        playerStats.visualEffects.emplace_back(color);
    }
    else
    {
        playerStats.visualEffects.emplace_back(ability.durationAmount, color);
    }
}

void bee::AbilitySystem::AddVisualEffectForStatusEffect(const AbilitySettings& ability, PlayerStatsComponent& playerStats)
{
    if (ability.statusEffectType == StatusEffectType::Invincibility)
    {
        playerStats.visualEffects.emplace_back(ability.durationAmount, m_invincibilityColor);
    }
    else
    {
        playerStats.visualEffects.emplace_back(ability.durationAmount, m_stunRootSilenceColor);
    }
}
