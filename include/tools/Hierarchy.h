#pragma once
#include <array>
#include <entt/entity/entity.hpp>

#include "imgui.h"

namespace bee
{

class Hierarchy
{
public:
    Hierarchy(entt::registry& registry);

    /**
     * \brief Displays the hierarchy through ImGui
     * and calculates the transforms for rendering.
     */
    void DisplayHierarchyAndCalculateTransforms();

    /**
     * \brief The hierarchy can only be initialized after models are loaded and constructed.
     * Call this function after loading and constructing the models.
     */
    void Init();

    /**
     * \brief Setting m_selectedNode.
     * \param entity The entity that will now be selected.
     */
    void SetSelected(entt::entity entity);

private:
    entt::registry& m_registry;

    entt::entity m_theoreticalRoot{entt::null};
    entt::entity m_selectedNode{entt::null};

    /**
     * \brief Recursive function to display all the children of an entity
     * \param parent The entity to display the children of
     * \param level To keep track of level of the hierarchy
     */
    void DisplayHierarchy(const entt::entity& parent, int level = 0);

    void CalculateTransforms(const entt::entity& parent);

    void DisplayEditor();

    void DestroyEntityAndChildren(entt::entity& parent);

    std::array<ImVec4, 6> m_textColors = {
        ImVec4(1.0f, 0.2f, 0.2f, 0.9f),  // 0 - red
        ImVec4(1.0f, 0.5f, 0.2f, 0.9f),  // 1 - orange
        ImVec4(1.0f, 1.0f, 0.4f, 0.9f),  // 2 - yellow
        ImVec4(0.2f, 1.0f, 0.3f, 0.9f),  // 3 - green
        ImVec4(0.2f, 0.7f, 1.0f, 0.9f),  // 4 - blue
        ImVec4(0.6f, 0.2f, 1.0f, 0.9f),  // 5 - purple
    };
};

}  // namespace bee
