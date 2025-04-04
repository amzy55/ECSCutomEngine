#pragma once
#include "imgui_entt_entity_editor.hpp"

namespace bee
{

class Inspector
{
public:
    Inspector();
    void Render();

private:
    MM::EntityEditor<entt::entity> m_editor;
    entt::entity m_entityToEdit{entt::null};
};

} //namespace bee
