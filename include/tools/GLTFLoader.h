#pragma once
#include <string>
#include <entt/entity/entity.hpp>

namespace entt {
enum class entity : std::uint32_t;
}

namespace tinygltf {
class Model;
}

namespace bee
{

class GLTFLoader
{
public:
    /**
     * \brief Loads a GLTF in memory without constructing it.
     * \param filePath The file path of the model.
     * \return True if the loading was successful, false otherwise.
     */
    [[maybe_unused]] bool Load(const std::string& filePath) const;  // NOLINT(modernize-use-nodiscard)

    /**
     * \brief Constructs a GLTF model if not constructed already or duplicates it otherwise
     * This is the function to call if you want to duplicate a model
     * \param filePath The file path of the model
     * \param rootEntity You can pass an entity if you want an entity to be associated with a model -
     * it will be the root entity of the model
     */
    void ConstructOrDuplicate(const std::string& filePath, entt::entity rootEntity = {entt::null}) const;

private:
    [[nodiscard]] entt::entity Construct(
        const std::string& filePath, tinygltf::Model& model, entt::entity rootEntity = {entt::null}) const;
    [[nodiscard]] entt::entity Duplicate(const entt::entity& entityToDuplicate, entt::entity rootEntity = {entt::null}) const;
    void CreateChildrenForDuplication(const entt::entity& parentToCopyIn, const entt::entity& parentOriginal) const;

    /**
     * \brief Loads and constructs a GLTF model.
     * \param filePath The file path of the model.
     */
    void LoadAndConstruct(const std::string& filePath) const;
};

} // namespace bee
