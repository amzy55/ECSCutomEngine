#include "resource_managers/MeshManager.h"

#include <memory>

#include "core/engine.hpp"
#include "rendering/simple_renderer.hpp"
#include "resource_managers/ResourceManager.h"
#include "tools/log.hpp"

bee::MeshManager::MeshManager()
{
    std::vector<unsigned int> indices = {// front
                                         0, 1, 2, 2, 3, 0,
                                         // right
                                         1, 5, 6, 6, 2, 1,
                                         // back
                                         7, 6, 5, 5, 4, 7,
                                         // left
                                         4, 0, 3, 3, 7, 4,
                                         // bottom
                                         4, 5, 1, 1, 0, 4,
                                         // top
                                         3, 2, 6, 6, 7, 3};
    // 8 elements
    std::vector<glm::vec3> vertices = {glm::vec3(-1.0, -1.0, 1.0), glm::vec3(1.0, -1.0, 1.0),   glm::vec3(1.0, 1.0, 1.0),
                                       glm::vec3(-1.0, 1.0, 1.0),  glm::vec3(-1.0, -1.0, -1.0), glm::vec3(1.0, -1.0, -1.0),
                                       glm::vec3(1.0, 1.0, -1.0),  glm::vec3(-1.0, 1.0, -1.0)};
    std::vector<glm::vec3> normals = {
        normalize(glm::vec3(-1.0, -1.0, 1.0)),
        normalize(glm::vec3(1.0, -1.0, 1.0)),
        normalize(glm::vec3(1.0, 1.0, 1.0)),
        normalize(glm::vec3(-1.0, 1.0, 1.0)),
        normalize(glm::vec3(-1.0, -1.0, -1.0)),
        normalize(glm::vec3(1.0, -1.0, -1.0)),
        normalize(glm::vec3(1.0, 1.0, -1.0)),
        normalize(glm::vec3(-1.0, 1.0, -1.0))};
    std::vector<glm::vec2> texCoords = {glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec2(1.f, 1.f),
                                        glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec2(1.f, 1.f)};
    std::vector<glm::vec3> colors = {// all white
                                     glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
                                     glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
                                     glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0)};

    m_separateMeshes["cube"] = std::make_shared<SimpleMesh>(indices, vertices, normals, texCoords, colors);

    // 24 vertices cube
    std::vector<unsigned int> indices24 = {
        0,  1,  2,  0,  2,  3,  // front
        4,  5,  6,  4,  6,  7,  // back
        8,  9,  10, 8,  10, 11, // top
        12, 13, 14, 12, 14, 15, // bottom
        16, 17, 18, 16, 18, 19, // right
        20, 21, 22, 20, 22, 23, // left
    };

    std::vector vertices24 = {
        glm::vec3(-1.0, -1.0, 1.0),  glm::vec3(1.0, -1.0, 1.0),  glm::vec3(1.0, 1.0, 1.0),  glm::vec3(-1.0, 1.0, 1.0),
        glm::vec3(-1.0, -1.0, -1.0), glm::vec3(-1.0, 1.0, -1.0), glm::vec3(1.0, 1.0, -1.0), glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(-1.0, 1.0, -1.0),  glm::vec3(-1.0, 1.0, 1.0),  glm::vec3(1.0, 1.0, 1.0),  glm::vec3(1.0, 1.0, -1.0),
        glm::vec3(-1.0, -1.0, -1.0), glm::vec3(1.0, -1.0, -1.0), glm::vec3(1.0, -1.0, 1.0), glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(1.0, -1.0, -1.0),  glm::vec3(1.0, 1.0, -1.0),  glm::vec3(1.0, 1.0, 1.0),  glm::vec3(1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, -1.0), glm::vec3(-1.0, -1.0, 1.0), glm::vec3(-1.0, 1.0, 1.0), glm::vec3(-1.0, 1.0, -1.0)};

    std::vector normals24 = {normalize(glm::vec3(0.0f, 0.0f, 1.0f)),  normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
                                        normalize(glm::vec3(0.0f, 0.0f, 1.0f)),  normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
                                        normalize(glm::vec3(0.0f, 0.0f, -1.0f)), normalize(glm::vec3(0.0f, 0.0f, -1.0f)),
                                        normalize(glm::vec3(0.0f, 0.0f, -1.0f)), normalize(glm::vec3(0.0f, 0.0f, -1.0f)),
                                        normalize(glm::vec3(0.0f, 1.0f, 0.0f)),  normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
                                        normalize(glm::vec3(0.0f, 1.0f, 0.0f)),  normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
                                        normalize(glm::vec3(0.0f, -1.0f, 0.0f)), normalize(glm::vec3(0.0f, -1.0f, 0.0f)),
                                        normalize(glm::vec3(0.0f, -1.0f, 0.0f)), normalize(glm::vec3(0.0f, -1.0f, 0.0f)),
                                        normalize(glm::vec3(1.0f, 0.0f, 0.0f)),  normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
                                        normalize(glm::vec3(1.0f, 0.0f, 0.0f)),  normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
                                        normalize(glm::vec3(-1.0f, 0.0f, 0.0f)), normalize(glm::vec3(-1.0f, 0.0f, 0.0f)),
                                        normalize(glm::vec3(-1.0f, 0.0f, 0.0f)), normalize(glm::vec3(-1.0f, 0.0f, 0.0f))};

    std::vector texCoords24 = {glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec2(1.f, 1.f),
                                          glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec2(1.f, 1.f),
                                          glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec2(1.f, 1.f),
                                          glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec2(1.f, 1.f),
                                          glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec2(1.f, 1.f),
                                          glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec2(1.f, 1.f)};
    std::vector colors24 = {// all white
                                       glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
                                       glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
                                       glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
                                       glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
                                       glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
                                       glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
                                       glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0)};

    m_separateMeshes["cube24"] = std::make_shared<SimpleMesh>(indices24, vertices24, normals24, texCoords24, colors24);
}

void bee::MeshManager::Add(const std::pair<std::string, int>& modelFilePathAndMeshIndex, const SimpleMesh& mesh)
{
    if (Engine.ResourceManager().GetModelManager().m_gltfModels.find(modelFilePathAndMeshIndex.first) !=
        Engine.ResourceManager().GetModelManager().m_gltfModels.end())
    {
        if (m_gltfMeshes.find(modelFilePathAndMeshIndex) == m_gltfMeshes.end())
        {
            m_gltfMeshes[modelFilePathAndMeshIndex] = std::make_shared<SimpleMesh>(mesh);
        }
        else
        {
            Log::Error("Attempted to add a mesh that already exists.\a");
        }
    }
    else
    {
        Log::Error("Attempted to add a mesh of a model that doesn't exist.\a");
    }
}

void bee::MeshManager::Add(const std::string& meshFilePath)
{
    if (m_separateMeshes.find(meshFilePath) == m_separateMeshes.end())
    {
        m_separateMeshes[meshFilePath] = std::make_shared<SimpleMesh>(meshFilePath);
    }
    else
    {
        Log::Error("Attempted to add a mesh that already exists.\a");
    }
}

std::optional<std::shared_ptr<bee::SimpleMesh>> bee::MeshManager::Get(
    const std::pair<std::string, int>& modelFilePathAndMeshIndex)
{
    if (const auto it = m_gltfMeshes.find(modelFilePathAndMeshIndex); it != m_gltfMeshes.end())
    {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<bee::SimpleMesh>> bee::MeshManager::Get(const std::string& meshFilePathOrName)
{
    if (const auto it = m_separateMeshes.find(meshFilePathOrName); it != m_separateMeshes.end())
    {
        return it->second;
    }
    return std::nullopt;
}
