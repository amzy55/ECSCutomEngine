#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace tinygltf
{
class Model;
}

namespace bee
{
class SimpleMesh;

class MeshManager
{
public:
    MeshManager();

    /**
     * \brief Adds a mesh from a GLTF.
     * It checks if the GLTF exists and if the mesh does not exist already.
     * \param modelFilePathAndMeshIndex Pair of:
     * - model (string -> file path of model)
     * - mesh index (int -> model.meshes[mesh index])
     * \param mesh The SimpleMesh to add.
     */
    void Add(const std::pair<std::string, int>& modelFilePathAndMeshIndex, const SimpleMesh& mesh);

    /**
     * \brief Adds a mesh that is not from a GLTF.
     * Example: an obj or a mesh created in code.
     * \param meshFilePathOrName Depends:
     * - obj -> file path
     * - in code -> name
     */
    void Add(const std::string& meshFilePathOrName);

    /**
     * \brief Gets a mesh from a GLTF.
     * It h
     * \param modelFilePathAndMeshIndex Pair of:
     * - model (string -> file path of model)
     * - mesh index (int -> model.meshes[mesh index])
     * \return It returns a SimpleMesh shared pointer if it finds it or nullopt if it does not.
     */
    [[nodiscard]] std::optional<std::shared_ptr<SimpleMesh>> Get(const std::pair<std::string, int>& modelFilePathAndMeshIndex);

    /**
     * \brief Gets a mesh that is not from a GLTF.
     * \param meshFilePathOrName Depends:
     * - obj -> file path
     * - in code -> name
     * \return It returns a SimpleMesh shared pointer if it finds it or nullopt if it does not.
     */
    [[nodiscard]] std::optional<std::shared_ptr<SimpleMesh>> Get(const std::string& meshFilePathOrName);

private:
    friend class ResourceManager;
    /**
     * \brief To get a mesh of a GLTF you need:
     * - model (string -> file path of model)
     * - mesh index (int -> model.meshes[mesh index])
     */
    std::map<std::pair<std::string, int>, std::shared_ptr<SimpleMesh>> m_gltfMeshes;

    // For storage of meshes that are not from a GLTF
    std::unordered_map<std::string, std::shared_ptr<SimpleMesh>> m_separateMeshes;
};

} //namespace bee
