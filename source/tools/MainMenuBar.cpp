#include "tools/MainMenuBar.h"

#include <imgui.h>

void bee::MainMenuBar::Display(const float& dt)
{
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Engine View"))
    {
        if (ImGui::MenuItem("FPS", nullptr, m_windowsToDisplayFlags & WindowsToDisplay::FPS))
        {
            m_windowsToDisplayFlags ^= WindowsToDisplay::FPS;
        }
        if (ImGui::MenuItem("Emitter Menu", nullptr, m_windowsToDisplayFlags & WindowsToDisplay::EmitterMenu))
        {
            m_windowsToDisplayFlags ^= WindowsToDisplay::EmitterMenu;
        }
        if (ImGui::MenuItem("Resource Manager", nullptr, m_windowsToDisplayFlags & WindowsToDisplay::ResourceManager))
        {
            m_windowsToDisplayFlags ^= WindowsToDisplay::ResourceManager;
        }
        if (ImGui::MenuItem("Hierarchy", nullptr, m_windowsToDisplayFlags & WindowsToDisplay::Hierarchy))
        {
            m_windowsToDisplayFlags ^= WindowsToDisplay::Hierarchy;
        }
        ImGui::EndMenu();
    }
    ImGui::Text("|");
    if (ImGui::BeginMenu("Ability System View"))
    {
        if (ImGui::MenuItem("Ability Menu", nullptr, m_windowsToDisplayFlags & WindowsToDisplay::AbilityMenu))
        {
            m_windowsToDisplayFlags ^= WindowsToDisplay::AbilityMenu;
        }
        if (ImGui::MenuItem("Player Stats", nullptr, m_windowsToDisplayFlags & WindowsToDisplay::PlayerStats))
        {
            m_windowsToDisplayFlags ^= WindowsToDisplay::PlayerStats;
        }
        if (ImGui::MenuItem("Visual Effects Editor", nullptr, m_windowsToDisplayFlags & WindowsToDisplay::VisualEffectsEditor))
        {
            m_windowsToDisplayFlags ^= WindowsToDisplay::VisualEffectsEditor;
        }
        ImGui::EndMenu();
    }
    ImGui::Text("|");
    ImGui::EndMainMenuBar();
}
