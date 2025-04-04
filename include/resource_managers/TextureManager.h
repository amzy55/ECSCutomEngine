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
struct SimpleTexture;

class TextureManager
{
public:
    TextureManager();

    /**
     * \brief Adds a texture from a GLTF.
     * It checks if the GLTF exists and if the texture does not exist already.
     * \param modelFilePathAndTextureIndex Pair of:
     * - model (string -> file path of model)
     * - texture index (int -> model.materials[texture index])
     * \param texture The SimpleTexture to add.
     */
    void Add(const std::pair<std::string, int>& modelFilePathAndTextureIndex, const SimpleTexture& texture);

    /**
     * \brief Adds a texture that is not from a GLTF.
     * \param textureFilePath The file path to the texture.
     */
    void Add(const std::string& textureFilePath);

    /**
     * \brief Gets a texture from a GLTF.
     * \param modelFilePathAndTextureIndex Pair of:
     * - model (string -> file path of model)
     * - texture index (int -> model.materials[texture index])
     * \return It returns a SimpleTexture shared pointer if it finds it or nullopt if it does not.
     */
    [[nodiscard]] std::optional<std::shared_ptr<SimpleTexture>> Get(
        const std::pair<std::string, int>& modelFilePathAndTextureIndex);

    /**
     * \brief Gets a texture that is not from a GLTF.
     * \param textureFilePath The file path to the texture.
     * \return It returns a SimpleTexture shared pointer if it finds it or nullopt if it does not.
     */
    [[nodiscard]] std::optional<std::shared_ptr<SimpleTexture>> Get(
        const std::string& textureFilePath);

private:
    friend class ResourceManager;
    /**
     * \brief To get a texture of GLTF you need:
     * - model (string -> file path of model)
     * - texture index (int -> model.materials[texture index])
     */
    std::map<std::pair<std::string, int>, std::shared_ptr<SimpleTexture>> m_gltfTextures;

    // For storage of textures that are not from a GLTF
    std::unordered_map<std::string, std::shared_ptr<SimpleTexture>> m_separateTextures;
};

} // namespace bee