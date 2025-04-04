#include "resource_managers/ModelManager.h"

#include <tinygltf/tiny_gltf.h>

void bee::ModelManager::Add(const std::string& filePath, const tinygltf::Model& model)
{
    m_gltfModels[filePath] = std::make_shared<tinygltf::Model>(model);
}

std::optional<std::shared_ptr<tinygltf::Model>> bee::ModelManager::Get(const std::string& filePath)
{
    if (const auto it = m_gltfModels.find(filePath); it != m_gltfModels.end())
    {
        return it->second;
    }
    return std::nullopt;
}

void bee::ModelManager::AddEntity(const std::string& filePath, const entt::entity& entity)
{
    m_filePathsToEntities[filePath].push_back(entity); }

void bee::ModelManager::RemoveEntity(const entt::entity& entityToRemove)
{
    for (auto& [path, entities] : m_filePathsToEntities)
    {
        for (auto it = entities.begin(); it != entities.end(); ++it)
        {
            if (*it == entityToRemove)
            {
                entities.erase(it);
                return;
            }
        }
    }
}

std::optional<entt::entity> bee::ModelManager::GetEntityFromFilePath(const std::string& filePath)
{
    if (const auto it = m_filePathsToEntities.find(filePath); it != m_filePathsToEntities.end())
    {
        return it->second[0]; // first entity in the vector - it does not matter which entity
    }
    return std::nullopt;
}

std::optional<std::string> bee::ModelManager::GetFilePathFromEntity(const entt::entity& entity)
{
    for (const auto& pair : m_filePathsToEntities)
    {
        for (auto& modelEntity : pair.second)
        {
            if (modelEntity == entity)
            {
                return pair.first;
            }
        }
    }
    return std::nullopt;
}
