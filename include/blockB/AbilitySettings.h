#pragma once

namespace bee
{

enum class Target
{
    Hostile = 0,
    Self,
    Friendly,
    SelfAndFriendly
};

enum class CastType
{
    Projectile = 0,
    RadiusAroundPlayer
};

enum class ProjectileShootType
{
    Line = 0,
    Cone
};

enum class ApplyType
{
    Instant = 0,
    EffectOverTime,
    BuffDebuff,
    StatusEffect
};

enum class InstantType
{
    Effect = 0,
    Dash
};

enum class StatusEffectType
{
    Stun = 0,
    Root,
    Silence,
    Invincibility
};

enum class Stat
{
    Health = 0,
    Ammo,
    ReloadSpeed,
    MovementSpeed,
    DealtDamageModifier,
    ReceivedDamageReduction
};

enum class IncreaseOrDecrease
{
    Increase = 0,
    Decrease
};

enum class AppliedAs
{
    Percentage = 0,
    Flat
};

struct AbilitySettings
{
    Target targetTeam = Target::Hostile;
    float aoeRadius = 0.f;
    int numberOfProjectiles = 1;
    float projectileRange = 0.f;
    float projectileRadius = 0.f;
    float projectileSpeed = 0.f;
    ProjectileShootType shootType = ProjectileShootType::Line;
    float spaceBetweenProjectiles = 0.f;
    float coneAngle = 0.f;
    CastType castType = CastType::Projectile;
    ApplyType applyType = ApplyType::Instant;
    InstantType instantType = InstantType::Effect;
    float dashSpeed = 0.f;
    StatusEffectType statusEffectType = StatusEffectType::Stun;
    Stat statAffected = Stat::Health;
    float statAffectAmount = 0.f;
    IncreaseOrDecrease increaseOrDecrease = IncreaseOrDecrease::Decrease;
    bool clampToMax = true;
    AppliedAs appliedAs = AppliedAs::Percentage;
    float durationAmount = 0.f;
    int numberOfTicks = 0;
    float aoeDuration = 0.2f;
};

}
