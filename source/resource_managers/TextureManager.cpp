#include "resource_managers/TextureManager.h"

#include "core/engine.hpp"
#include "rendering/simple_renderer.hpp"
#include "resource_managers/ResourceManager.h"
#include "tools/log.hpp"

bee::TextureManager::TextureManager()
{
    // always store a white texture to use for rendering in case a model doesn't have textures
    m_separateTextures["white"] = std::make_shared<SimpleTexture>("textures/white.png");
    m_separateTextures["giorno"] = std::make_shared<SimpleTexture>("textures/highrezgiorno1024.png");
}

void bee::TextureManager::Add(const std::pair<std::string, int>& modelFilePathAndTextureIndex, const SimpleTexture& texture)
{
    if (Engine.ResourceManager().GetModelManager().m_gltfModels.find(modelFilePathAndTextureIndex.first) !=
        Engine.ResourceManager().GetModelManager().m_gltfModels.end())
    {
        if (m_gltfTextures.find(modelFilePathAndTextureIndex) == m_gltfTextures.end())
        {
            m_gltfTextures[modelFilePathAndTextureIndex] = std::make_shared<SimpleTexture>(texture);
        }
        else
        {
            Log::Error("Attempted to add a texture that already exists.\a");
        }
    }
    else
    {
        Log::Error("Attempted to add a texture of a model that doesn't exist.\a");
    }
}

void bee::TextureManager::Add(const std::string& textureFilePath)
{
    if (m_separateTextures.find(textureFilePath) == m_separateTextures.end())
    {
        m_separateTextures[textureFilePath] = std::make_shared<SimpleTexture>(textureFilePath);
    }
    else
    {
        Log::Error("Attempted to add a texture that already exists.\a");
    }
}

std::optional<std::shared_ptr<bee::SimpleTexture>> bee::TextureManager::Get(
    const std::pair<std::string, int>& modelFilePathAndTextureIndex)
{
    if (const auto it = m_gltfTextures.find(modelFilePathAndTextureIndex); it != m_gltfTextures.end())
    {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<bee::SimpleTexture>> bee::TextureManager::Get(const std::string& textureFilePath)
{
    if (const auto it = m_separateTextures.find(textureFilePath); it != m_separateTextures.end())
    {
        return it->second;
    }
    return std::nullopt;
}
