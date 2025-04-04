#include "resource_managers/ResourceManager.h"

#include <imgui.h>

#include "core/engine.hpp"
#include "ecs/ECS.h"
#include "ecs/components/PlayerStatsComponents.h"
#include "tools/GLTFLoader.h"
#include "tools/MainMenuBar.h"
#include "tools/tools.hpp"

void bee::ResourceManager::ImGuiWindow()
{
    if (!Engine.MainMenuBar().IsFlagOn(WindowsToDisplay::ResourceManager))
        return;

    ImGui::SetNextWindowSize({350.f, 300.f}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({1920.f - 350.f, 18.f}, ImGuiCond_Once);
    ImGui::Begin("Resources");
    if (ImGui::CollapsingHeader("Abilities", ImGuiTreeNodeFlags_DefaultOpen))
    {
        std::string abilityToDelete;
        for (const auto& [name, abilitySettings] : m_abilityManager.m_abilities)
        {
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine();
            ImGui::PushID(name.c_str());
            if (ImGui::Button("Delete", ImVec2{50.f, 18.f}))
            {
                abilityToDelete = name;
            }
            ImGui::PopID();
        }
        if (!abilityToDelete.empty())
        {
            // remove from players
            auto view = Engine.ECS().Registry().view<AbilitiesOnPlayerComponent>();
            for (auto [player, abilitiesComponent] : view.each())
            {
                for (auto it = abilitiesComponent.abilityNamesToInputsMap.begin();
                     it != abilitiesComponent.abilityNamesToInputsMap.end();)
                {
                    if (it->first == abilityToDelete)
                    {
                        it = abilitiesComponent.abilityNamesToInputsMap.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
            // remove from manager
            m_abilityManager.Remove(abilityToDelete);
        }
    }
    if (ImGui::CollapsingHeader("Models", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Click on a model to spawn it in");
        for (const auto& [path, model] : m_modelManager.m_gltfModels)
        {
            auto buttonText = StringRemove(StringRemove(path, "assets/models/"), ".gltf");
            if (ImGui::Button(buttonText.c_str(), ImGui::CalcTextSize(buttonText.c_str()).plus(ImVec2{7.0f})))
            {
                Engine.GLTFLoader().ConstructOrDuplicate(path);
            }
        }
    }
    if (ImGui::CollapsingHeader("Meshes", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto i : m_meshManager.m_separateMeshes)
        {
            ImGui::Text("%s", i.first.c_str());
        }
        for (auto i : m_meshManager.m_gltfMeshes)
        {
            ImGui::Text(
                "%s", 
                (StringRemove(StringRemove(i.first.first, "assets/models/"), ".gltf") + " - mesh " + std::to_string(i.first.second))
                    .c_str());
        }
    }
    if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto i : m_textureManager.m_separateTextures)
        {
            ImGui::Text("%s", i.first.c_str());
        }
        for (auto i : m_textureManager.m_gltfTextures)
        {
            ImGui::Text(
                "%s", (StringRemove(StringRemove(i.first.first, "assets/models/"), ".gltf") + " - texture " +
                         std::to_string(i.first.second))
                            .c_str());
        }
    }
    ImGui::End();
}
