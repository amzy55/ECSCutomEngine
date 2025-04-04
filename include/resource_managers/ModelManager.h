#pragma once
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace entt {
enum class entity : std::uint32_t;
}

namespace tinygltf
{
class Model;
}

namespace bee
{
class SimpleMesh;

class ModelManager
{
public:
    /**
     * \brief Adds a tinyGLTF model.
     * \param filePath The file path of the model.
     * \param model The model to add.
     */
    void Add(const std::string& filePath, const tinygltf::Model& model);

    /**
     * \brief Gets a tinyGLTF model.
     * \param filePath The file path of the model.
     * \return It returns a tinyGLTF model shared pointer if it finds it or nullopt if it does not.
     */
    [[nodiscard]] std::optional<std::shared_ptr<tinygltf::Model>> Get(const std::string& filePath);

    /**
     * \brief Stores what is the root entity for a model for duplication purposes.
     * \param filePath The file path of the model.
     * \param entity The root entity.
     */
    void AddEntity(const std::string& filePath, const entt::entity& entity);

    /**
     * \brief Removes an entity from m_filePathsToEntities on destruction.
     * \param entityToRemove The entity to remove.
     */
    void RemoveEntity(const entt::entity& entityToRemove);

    /**
     * \brief Returns the entity associated with a model.
     * \param filePath The file path of the model.
     * \return It returns the entity if it finds it or nullopt if it does not.
     */
    [[nodiscard]] std::optional<entt::entity> GetEntityFromFilePath(const std::string& filePath);

    /**
     * \brief Returns the file path of a model associated with an entity.
     * \param entity The root entity of the model. It must have a RootComponent.
     * \return It returns the file path if it finds it or nullopt if it does not.
     */
    [[nodiscard]] std::optional<std::string> GetFilePathFromEntity(const entt::entity& entity);

private:
    friend class ResourceManager;
    friend class MeshManager;
    friend class TextureManager;
    std::unordered_map<std::string, std::shared_ptr<tinygltf::Model>> m_gltfModels;
    // whenever a a model is constructed or duplicated, it will be added to this map
    std::unordered_map<std::string, std::vector<entt::entity>> m_filePathsToEntities;
};

} // namespace bee