#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "xsr.hpp"
#include "entt/entity/entity.hpp"

namespace bee
{

class Resources;

class SimpleMesh
{
public:
    /// Loads a mesh from a obj file
    SimpleMesh(const std::string& path);

    /// Loads a mesh with a given name from a obj file
    SimpleMesh(const std::string& path, const std::string& object);

    /// Creates a mesh from vertex/index data
    SimpleMesh(
        const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texture_coordinates,
        const std::vector<glm::vec3>& colors);

    /// Creates a mesh from xsr mesh handle
    SimpleMesh(const xsr::mesh_handle& handle);

    xsr::mesh_handle handle;
};

struct SimpleTexture
{
public:
    SimpleTexture(const std::string& path);
    SimpleTexture(const xsr::texture_handle& handle);

    xsr::texture_handle handle;
};

//struct SimpleCamera
//{
//    glm::mat4 projection = glm::mat4(1.f);
//    glm::mat4 view = glm::mat4(1.f);
//
//    float moveSpeed = 5.f;
//};

struct SimpleDirectionalLight
{
    glm::vec3 direction;
    glm::vec3 color;
};

struct SimpleMeshRender
{
    std::shared_ptr<SimpleMesh> mesh;
    std::shared_ptr<SimpleTexture> texture;
    glm::vec4 mulColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // -1 is black, 0 is grey, 1 is white
    glm::vec4 addColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
};

class SimpleRenderer
{
public:
    SimpleRenderer();
    ~SimpleRenderer();

    /**
     * \brief Render the scene.
     */
    void Render();
    
    /**
     * \brief Render a single mesh. Render() still needs to be called.
     * \param mesh The mesh to render
     * \param transform The transform matrix of the mesh
     */
    void RenderMesh(SimpleMeshRender* mesh, glm::mat4 transform, glm::vec4 addColor);
    
    /**
     * \brief The camera needs to be initialized.
     */
    void InitCamera();

private:
    std::vector<SimpleDirectionalLight> m_lights;
};

} // namespace bee