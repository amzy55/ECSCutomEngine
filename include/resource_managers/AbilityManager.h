#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include "blockB/AbilitySettings.h"

namespace bee
{

class AbilityManager
{
public:
    void Add(const std::string& name, const AbilitySettings& abilitySettings);
    [[nodiscard]] std::optional<AbilitySettings> Get(const std::string& name);
    void Remove(const std::string& name);

private:
    friend class Serializer;
    friend class ResourceManager;
    friend class AbilityCreationMenu;
    friend class PlayerStatsWindows;
    std::unordered_map<std::string, AbilitySettings> m_abilities;

};

}
