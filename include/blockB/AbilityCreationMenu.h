#pragma once
#include <string>
#include <unordered_map>

namespace bee
{
struct AbilitySettings;

class AbilityCreationMenu
{
public:
    void Display();

private:
    // name
    char m_name[32] = "";

    // targets
    //std::vector<const char*> m_targets = {"Hostile", "Self", "Friendly", "Self & Friendly"}; // to figure out later
    const char* m_targets[4] = {"Hostile", "Self", "Friendly", "Self & Friendly"};
    int m_targetsSelectedItem = 0;

    // targeting style
    const char* m_castTypes[2] = {"Projectile", "Area of Effect (Radius around Player)"};
    int m_castTypesSelectedItem = 0;
    float m_aoeRadius = 0.0f;
    int m_numberOfProjectiles = 1;
    float m_projectileRange = 20.0f;
    float m_projectileRadius = 0.5f;
    float m_projectileSpeed = 15.0f;

    // projectile shoot type
    const char* m_projectileShootTypes[2] = {"Line", "Cone"};
    int m_projectileShootTypesSelectedItem = 1;
    float m_spaceBetweenProjectiles = 2.f;
    float m_coneAngle = 0.f;

    // apply type
    const char* m_applyTypes[4] = {"Instant", "Effect Over Time", "Buff / Debuff", "Status Effect"};
    int m_applyTypesSelectedItem = 0;

    // instant type
    const char* m_instantTypes[2] = {"Effect", "Dash"};
    int m_instantTypesSelectedItem = 0;

    // dash
    float m_dashSpeed = 200.f;

    // statusEffect type
    const char* m_statusEffectTypes[4] = {"Stun", "Root", "Silence", "Invincibility"};
    const char* m_statusEffectToolTips[4] = {"Cannot perform any actions.", "Cannot move.", "Cannot cast abilities.", "Immune to any effects."};
    int m_statusEffectTypesSelectedItem = 0;

    // ticks
    float m_duration = 0.f;
    int m_numberOfTicks = 0;

    // stats
    const char* m_stats[6] = {"Health", "Ammo", "Reload Speed", "Movement Speed", "Dealt Damage Modifier %", "Received Damage Reduction %"};
    int m_statsSelectedItem = 0;

    // increase or decrease
    const char* m_increaseOrDecrease[2] = {"Increase", "Decrease"};
    int m_increaseOrDecreaseSelectedItem = 1;

    // clamp to max
    bool m_clampToMax = true;

    // applied as
    const char* m_appliedAs[2] = {"Percentage", "Flat"};
    int m_appliedAsSelectedItem = 1;

    float m_amount = 0.0f;

    // errors on save
    std::unordered_map<std::string, bool> m_errors;

    // warnings on save
    std::unordered_map<std::string, bool> m_warnings;

    // edit
    std::string m_editSelectedItem;

    // inside Display()
    void DisplayAbilityCreationSettingsWindow();
    void DisplayWarningsWindow();
    void DisplayConfirmOrOverrideWindow();

    // inside DisplayAbilityCreationSettingsWindow()
    void NameSection();
    void TargetingProfileSection();
    void ApplyTypeSection();
    void DashSection();
    void EffectSection();
    void OnSaveClick();
    void DisplayErrors();
    void DisplayEditCombo();

    // inside OnSaveClick()
    [[nodiscard]] bool CheckForErrors();
    [[nodiscard]] bool CheckForWarnings();

    [[nodiscard]] bool ConfirmOverrideWindow();
    bool m_displayConfirmOverrideWindow = false;
    bool m_doNotShowConfirmOverrideWindowAgain = false;

    [[nodiscard]] bool WarningsWindow();
    bool m_displayWarningsWindow = false;
    bool m_doNotShowWarningsWindowAgain = false;

    void DisplayConfirmOverrideWindowOrSaveOrOverrideAbility();

    void SaveOrOverrideAbility();

    void SetSettingsForEdit(const std::string& name, const AbilitySettings& settings);
};
}
