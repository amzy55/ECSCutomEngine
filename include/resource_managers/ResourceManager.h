#pragma once
#include "resource_managers/ModelManager.h"
#include "resource_managers/MeshManager.h"
#include "resource_managers/TextureManager.h"
#include "resource_managers/AbilityManager.h"

namespace bee
{
class ResourceManager
{
public:
    // all managers are publicly accessible
    ModelManager& GetModelManager() { return m_modelManager; }
    MeshManager& GetMeshManager() { return m_meshManager; }
    TextureManager& GetTextureManager() { return m_textureManager; }
    AbilityManager& GetAbilityManager() { return m_abilityManager; }

    /**
     * \brief Displays the resource viewer.
     */
    void ImGuiWindow();

private:
    ModelManager m_modelManager;
    MeshManager m_meshManager;
    TextureManager m_textureManager;
    AbilityManager m_abilityManager;
};

} //namespace bee
