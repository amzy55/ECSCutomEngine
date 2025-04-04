#pragma once
#include <imgui.h>

#include "core/engine.hpp"
#include "MainMenuBar.h"

namespace bee
{
/**
 * \brief Prints the FPS in a ImGui window
 * Can only be used with prior ImGui initialization
 * \param dt Delta time for calculations
 * \param refreshTime How often you want the FPS to update
 */
inline void PrintFPS(const float& dt, const float refreshTime = 0.3f)
{
    if (!Engine.MainMenuBar().IsFlagOn(WindowsToDisplay::FPS))
        return;

    static float fps = 0.f;
    static float ms = 0.f;
    static float fpsTimer = refreshTime;

    fpsTimer += dt;
    if (fpsTimer > refreshTime)
    {
        fps = 1.f / dt;
        ms = dt * 1000.f;
        fpsTimer = 0.f;
    }

    ImGui::SetNextWindowSize({350.f, 100.f});
    ImGui::SetNextWindowPos({0.f, 650.f + 300.f}, ImGuiCond_Once);
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("FPS: %.2f", fps);
    ImGui::Text("Ms: %f", ms);
    ImGui::End();
}
} // namespace bee
