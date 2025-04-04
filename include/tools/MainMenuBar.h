#pragma once

namespace bee
{

struct WindowsToDisplay
{
    enum Enum : unsigned int
    {
        // engine
        FPS = 1 << 0,
        EmitterMenu = 1 << 1,
        ResourceManager = 1 << 2,
        Hierarchy = 1 << 3,
        // ability menu
        AbilityMenu = 1 << 4,
        PlayerStats = 1 << 5,
        VisualEffectsEditor = 1 << 6,
        All = ~0u // negates all 0s to 1s
    };
};
// doing this instead of enum class so that I don't need to do casts

class MainMenuBar
{
public:
    [[nodiscard]] unsigned int GetFlags() const { return m_windowsToDisplayFlags; }
    [[nodiscard]] bool IsFlagOn(const WindowsToDisplay::Enum flag) const { return m_windowsToDisplayFlags & flag; }
    void Display(const float& dt);

private:
    friend class Serializer;
    unsigned int m_windowsToDisplayFlags = WindowsToDisplay::ResourceManager | WindowsToDisplay::Hierarchy |
                                           WindowsToDisplay::AbilityMenu | WindowsToDisplay::PlayerStats;

};

}