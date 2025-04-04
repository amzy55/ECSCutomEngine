#pragma once
#include <map>
#include <glm/vec4.hpp>
#include <string>

#include "BaseSystem.h"

namespace bee
{
enum class Stat;
enum class IncreaseOrDecrease;
struct PlayerStatsComponent;
struct MeshComponent;
struct PhysicsBody2DComponent;
struct TransformComponent;
struct AbilitySettings;
struct AbilityComponent;

class AbilitySystem final : public BaseSystem
{
public:
    explicit AbilitySystem(entt::registry& registry);

    void Update(const float& dt) override;

private:
    std::vector<glm::vec4> m_teamColors = {
        {0.5f, 0.75f, 1.f, 1.f}, // 0
        {1.f, 0.5f, 0.75f, 1.f}, // 1
        {0.5f, 1.f, 0.75f, 1.f}, // 2
        {1.f, 1.f, 0.5f, 1.f},   // 3
    };
    std::map<std::pair<Stat, IncreaseOrDecrease>, glm::vec4> m_statColors;
    std::vector<std::string> m_statNamesForVisualEffectsEditWindow;
    std::vector<glm::vec4> m_statDefaultColors = {
        {1.f, 1.f, 1.f, 1.f}, // Increase - white
        {0.05f, 0.05f, 0.05f, 1.f}, // Decrease - black
    };
    glm::vec4 m_stunRootSilenceColor = {0.5f, 0.5f, 0.5f, 1.f}; // grey
    glm::vec4 m_invincibilityColor = {1.f, 1.f, 0.f, 1.f};      // yellow

    void UpdateAmmoAndDisplayVisualEffects(float dt);

    void UpdateAbilities(float dt);

    void CheckInputToCreateAbilities();

    void DisplayVisualEffectsEditor();

    void CreateAbility(const std::string& abilityName, entt::entity castByPlayer);

    std::pair<float&, float&> GetStat(Stat stat, PlayerStatsComponent& playerStats);

    /**
     * \brief Applies the amount based on stat type, increase or decrease and apply type.
     * \param ability Ability data.
     * \param hitPlayerStats The player stats to modify.
     * \param castByPlayerStats The player who cast the ability (for now only used for the damage modifier).
     * \param revert To revert the effect of the ability - for example after an ability with a duration ends.
     */
    void ApplyAmount(
        const AbilitySettings& ability, PlayerStatsComponent& hitPlayerStats,
        const PlayerStatsComponent& castByPlayerStats, bool revert);

    void ApplyStatusEffect(const AbilitySettings& ability, PlayerStatsComponent& playerStats);

    void AddToHierarchy(entt::entity entity, const std::string& name);

    void RemoveFromHierarchy(entt::entity entity);

    entt::entity CreateProjectile(const AbilityComponent& abilityComponent, const MeshComponent& meshComponent,
        const PhysicsBody2DComponent& physicsBody, const TransformComponent& transformComponent);

    void DestroyOrEraseComponentsBasedOnFirstProjectileInLine(entt::entity ability, AbilityComponent& abilityComponent);

    void ProjectilesInLineLogic(entt::entity ability, AbilityComponent& abilityComponent);

    void AddVisualEffectForApplyAmount(const AbilitySettings& ability, PlayerStatsComponent& playerStats);

    void AddVisualEffectForStatusEffect(const AbilitySettings& ability, PlayerStatsComponent& playerStats);
};
}

