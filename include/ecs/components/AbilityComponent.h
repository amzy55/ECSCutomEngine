#pragma once
#include <entt/entity/entity.hpp>
#include "blockB/AbilitySettings.h"

namespace bee
{

struct AbilityComponent
{
    entt::entity castByPlayer = entt::null;
    entt::entity hitPlayer = entt::null;
    AbilitySettings settings;
    int currentNumberOfProjectiles = 1; // for multiple projectiles shot in a line
    float currentProjectileRange = 0.f;
    float currentDurationAmount = settings.durationAmount;
    int currentNumberOfTicks = settings.numberOfTicks;
    bool firstProjectileInLine = false;
    bool dead = false;
    float currentAoeDuration = settings.aoeDuration;

    explicit AbilityComponent(const entt::entity& argCastByPlayer, const AbilitySettings& argSettings)
        : castByPlayer(argCastByPlayer), settings(argSettings)
    {
    }
};

}
