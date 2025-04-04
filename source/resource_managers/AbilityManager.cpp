#include "resource_managers/AbilityManager.h"

#include "tools/log.hpp"

void bee::AbilityManager::Add(const std::string& name, const AbilitySettings& abilitySettings)
{
    m_abilities[name] = abilitySettings;
}
    
std::optional<bee::AbilitySettings> bee::AbilityManager::Get(const std::string& name)
{
    if (const auto it = m_abilities.find(name); it != m_abilities.end())
    {
        return it->second;
    }
    return std::nullopt;
}

void bee::AbilityManager::Remove(const std::string& name)
{
    if (const auto it = m_abilities.find(name); it != m_abilities.end())
    {
        m_abilities.erase(it);
    }
    else
    {
        Log::Error("Attempted to erase an ability that does not exist: {}", name);
    }
}
