#pragma once
#include <string>
#include <optional>
#include <glm/vec4.hpp>
#include "core/input.hpp"

namespace bee
{

struct PlayerStatsComponent
{
    int id = -1;
    int teamId = -1;

    float baseHealth = 50.0f;
    float currentHealth = baseHealth;

    float baseAmmo = 3.f;
    float currentAmmo = baseAmmo;

    float baseReloadSpeed = 0.5f;
    float currentReloadSpeed = baseReloadSpeed;

    float baseMovementSpeed = 5.0f;
    float currentMovementSpeed = baseMovementSpeed;

    float baseDealtDamageModifier = 1.f;
    float currentDealtDamageModifier = baseDealtDamageModifier;

    float baseReceivedDamageReduction = 1.f;
    float currentReceivedDamageReduction = baseReceivedDamageReduction;

    enum State : unsigned int
    {
        Stunned = 1 << 0,
        Rooted = 1 << 1,
        Silenced = 1 << 2,
        Invincible = 1 << 3,
        Dashing = 1 << 4,
    };

    unsigned state = 0;

    struct VisualEffect
    {
        float duration = 0.2f;
        float currentDuration = duration;
        glm::vec4 color = glm::vec4(1.f);

        VisualEffect() = default;
        explicit VisualEffect(glm::vec4 argColor) : color(argColor) {}
        explicit VisualEffect(float argDuration, glm::vec4 argColor) : duration(argDuration), color(argColor) {} 
    };

    std::vector<VisualEffect> visualEffects;

    PlayerStatsComponent() = default;
    PlayerStatsComponent(const int argId, const int argTeamId) : id(argId), teamId(argTeamId) {}

    bool IsStatusEffectActive(State statusEffect) const { return state & statusEffect; }
};

struct AbilitiesOnPlayerComponent
{
    // ability name -> input for the ability (keyboard and controller)
    std::unordered_map<std::string, std::pair<std::optional<Input::KeyboardKey>, std::optional<Input::GamepadButton>>>
        abilityNamesToInputsMap;
    int maxNumberOfAbilities = 2;
};

}
