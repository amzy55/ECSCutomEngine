#include "tools/GLTFLoader.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 4018)
#pragma warning(disable : 4267)
#pragma warning(disable : 4101)
#pragma warning(disable : 4081)
#include "tinygltf/tiny_gltf.h"
#pragma warning(pop)

#include <tools/tools.hpp>
#include <entt/entity/entity.hpp>

#include "core/engine.hpp"
#include "ecs/ECS.h"
#include "rendering/simple_renderer.hpp"
#include "resource_managers/ResourceManager.h"
#include "tools/Hierarchy.h"
#include "tools/log.hpp"
#include "ecs/components/compact_includes/HierarchyIncludeComponents.h"

void bee::GLTFLoader::LoadAndConstruct(const std::string& filePath) const
{
    if (Load(filePath)) // if loading is successful
        ConstructOrDuplicate(filePath);
}

bool bee::GLTFLoader::Load(const std::string& filePath) const
{
    if (Engine.ResourceManager().GetModelManager().Get(filePath).has_value())
    {
        Log::Error("Model already loaded! If you want to duplicate a model call ConstructOrDuplicate().\a");
        return false;
    }

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string error;
    std::string warning;

    const bool result = loader.LoadASCIIFromFile(&model, &error, &warning, filePath);

    if (!warning.empty())
    {
        Log::Warn(warning);
    }
    if (!error.empty() || !result)
    {
        Log::Error("Loading GLTF model failed!\a" + error);
        return false;
    }

    Engine.ResourceManager().GetModelManager().Add(filePath, model);

    int meshIndex = -1;
    for (auto& mesh : model.meshes)
    {
        meshIndex++;
        // should only have one primitive
        for (const auto& primitive : mesh.primitives)
        {
            std::vector<glm::vec3> positionsVector;
            std::vector<glm::vec3> normalsVector;
            std::vector<glm::vec2> texCoordsVector;
            std::vector<unsigned int> indicesVector;
            std::vector<glm::vec3> colorsVector;

            if (primitive.attributes.find("POSITION") != primitive.attributes.end())
            {
                int index = primitive.attributes.at("POSITION");
                const tinygltf::Accessor& accessor = model.accessors[index];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const auto* positionData =
                    reinterpret_cast<const glm::vec3*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                positionsVector = std::vector<glm::vec3>(positionData, positionData + accessor.count);
            }

            if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
            {
                int index = primitive.attributes.at("NORMAL");
                const tinygltf::Accessor& accessor = model.accessors[index];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const auto* normalsData =
                    reinterpret_cast<const glm::vec3*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                normalsVector = std::vector<glm::vec3>(normalsData, normalsData + accessor.count);
            }

            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
            {
                int index = primitive.attributes.at("TEXCOORD_0");
                const tinygltf::Accessor& accessor = model.accessors[index];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const auto* texCoordsData =
                    reinterpret_cast<const glm::vec2*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                texCoordsVector = std::vector<glm::vec2>(texCoordsData, texCoordsData + accessor.count);
            }

            if (primitive.indices != -1)
            {
                int indexAccessorIndex = primitive.indices;
                const tinygltf::Accessor& indexAccessor = model.accessors[indexAccessorIndex];
                const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

                for (size_t i = 0; i < indexAccessor.count; ++i)
                {
                    switch (indexAccessor.componentType)
                    {
                        case (TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT):
                        {
                            const auto* buffer = reinterpret_cast<const uint32_t*>(
                                &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                            indicesVector.push_back(buffer[i]);
                            break;
                        }
                        case (TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT):
                        {
                            const auto* buffer = reinterpret_cast<const uint16_t*>(
                                &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                            indicesVector.push_back(buffer[i]);
                            break;
                        }
                        case (TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE):
                        {
                            const auto* buffer = reinterpret_cast<const uint8_t*>(
                                &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                            indicesVector.push_back(buffer[i]);
                            break;
                        }
                        default:
                            Log::Error("Failure while reading index data\a");
                            break;
                    }
                }
            }

            colorsVector = std::vector<glm::vec3>(positionsVector.size(), glm::vec3(1.f));
            xsr::mesh_handle meshHandle = xsr::create_mesh(
                indicesVector.data(), static_cast<unsigned>(indicesVector.size()), &positionsVector.data()->x,
                &normalsVector.data()->x, &texCoordsVector.data()->x, &colorsVector.data()->x,
                static_cast<unsigned>(positionsVector.size()));
            auto simpleMesh = SimpleMesh(meshHandle);
            Engine.ResourceManager().GetMeshManager().Add(std::make_pair(filePath, meshIndex), simpleMesh);
        }
    }

    int materialIndex = -1;
    for (auto& material : model.materials)
    {
        materialIndex++;
        if (material.values.find("baseColorTexture") != material.values.end())
        {
            int textureIndex = material.values["baseColorTexture"].TextureIndex();
            const tinygltf::Texture& texture = model.textures[textureIndex];
            tinygltf::Image image = model.images[texture.source];
            // FlipImageVertically(image);

            xsr::texture_handle textureHandle = xsr::create_texture(image.width, image.height, image.image.data());
            auto simpleTexture = SimpleTexture(textureHandle);
            Engine.ResourceManager().GetTextureManager().Add(std::make_pair(filePath, textureIndex), simpleTexture);
        }
    }

    return true;
}

void bee::GLTFLoader::ConstructOrDuplicate(const std::string& filePath, entt::entity rootEntity) const
{
    const auto optionalModel = Engine.ResourceManager().GetModelManager().Get(filePath);
    if (optionalModel.has_value())
    {
        tinygltf::Model& model = *(optionalModel.value());

        entt::entity newEntity;
        if (const auto entityToDuplicate = Engine.ResourceManager().GetModelManager().GetEntityFromFilePath(filePath); entityToDuplicate.has_value())
        {
            newEntity = Duplicate(entityToDuplicate.value(), rootEntity);
            Engine.Hierarchy().SetSelected(newEntity);
        }
        else
        {
            newEntity = Construct(filePath, model, rootEntity);
        }

        Engine.ResourceManager().GetModelManager().AddEntity(filePath, newEntity);
        auto& registry = Engine.ECS().Registry();
        auto theoreticalRootView = registry.view<TheoreticalRootComponent>();
        auto& rootHierarchyComponent = registry.get<HierarchyComponent>(theoreticalRootView.front());
        auto& newEntityHierarchyComponent = registry.get<HierarchyComponent>(newEntity);
        newEntityHierarchyComponent.parent = theoreticalRootView.front();
        rootHierarchyComponent.children.push_back(newEntity);
    }
    else
    {
        Log::Error("Model not found when trying to construct or duplicate.\a");
    }
}

entt::entity bee::GLTFLoader::Construct(const std::string& filePath, tinygltf::Model& model, entt::entity rootEntity) const
{
    auto& registry = Engine.ECS().Registry();
    if (rootEntity == entt::null)
    {
        rootEntity = registry.create();
    }
    registry.emplace<RootComponent>(rootEntity);
    registry.emplace<TransformComponent>(rootEntity);
    auto& rootNameComponent = registry.emplace<NameComponent>(rootEntity);
    std::string modelName = bee::StringRemove(filePath, "assets/models/");
    modelName = bee::StringRemove(modelName, ".gltf");
    rootNameComponent.name = modelName;
    auto& rootHierarchyComponent = registry.emplace<HierarchyComponent>(rootEntity);

    std::unordered_map<int, entt::entity> nodeIndexToEntity;
    int nodeIndex = -1;
    for (auto& node : model.nodes)
    {
        entt::entity entity = registry.create();
        nodeIndex++;
        nodeIndexToEntity[nodeIndex] = entity;
        auto& transformComponent = registry.emplace<TransformComponent>(entity);
        auto& nameComponent = registry.emplace<NameComponent>(entity);
        registry.emplace<HierarchyComponent>(entity);

        // could potentially be improved
        if (!node.matrix.empty())
        {
            transformComponent.pos = {
                static_cast<float>(node.matrix[12]), static_cast<float>(node.matrix[13]), static_cast<float>(node.matrix[14])};
            glm::mat4 transformMatrix;
            for (int i = 0; i < 16; ++i)
            {
                transformMatrix[i / 4][i % 4] = static_cast<float>(node.matrix[i]);
            }

            glm::mat4 rotationMatrix(transformMatrix);
            transformComponent.rotation = rotationMatrix;

            transformComponent.scale = {
                glm::length(glm::vec3(transformMatrix[0])), glm::length(glm::vec3(transformMatrix[1])),
                glm::length(glm::vec3(transformMatrix[2]))};
        }
        else
        {
            if (!node.translation.empty())
            {
                transformComponent.pos = {
                    static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]),
                    static_cast<float>(node.translation[2])};
            }
            if (!node.rotation.empty())
            {
                transformComponent.rotation = {
                    static_cast<float>(node.rotation[3]), static_cast<float>(node.rotation[0]),
                    static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2])};
            }
            if (!node.scale.empty())
            {
                transformComponent.scale = {
                    static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]), static_cast<float>(node.scale[2])};
            }
        }

        if (!node.name.empty())
        {
            nameComponent.name = node.name;
        }
        else
        {
            nameComponent.name = "node_" + std::to_string(nodeIndex);
        }

        if (node.light != -1)
        {
            Log::Warn("Lights not supported yet");
        }

        if (node.skin != -1)
        {
            Log::Warn("Skins not supported yet");
        }

        if (node.emitter != -1)
        {
            Log::Warn("Audio emitters not supported yet");
        }

        if (node.mesh != -1)
        {
            // should only have one primitive
            for (const auto& primitive : model.meshes[node.mesh].primitives)
            {
                auto& meshComponent = registry.emplace<MeshComponent>(entity);

                auto optionalMesh = Engine.ResourceManager().GetMeshManager().Get(std::make_pair(filePath, node.mesh));
                if (optionalMesh.has_value())
                {
                    meshComponent.mesh = optionalMesh.value();
                }
                else
                {
                    Log::Error("Attempted to use a mesh that doesn't exist.\a");
                }

                int materialIndex = primitive.material;
                auto optionalTexture =
                    Engine.ResourceManager().GetTextureManager().Get(std::make_pair(filePath, materialIndex));
                if (optionalTexture.has_value())
                {
                    meshComponent.texture = optionalTexture.value();
                }
                else
                {
                    // applying a white texture so that the model can still be loaded
                    // although a possible issue is that i am relying on the
                    // white texture to exist on disk for my engine to properly work
                    meshComponent.texture = Engine.ResourceManager().GetTextureManager().Get("white").value();
                    // Log::Warn("Texture not found. Using white texture instead.");
                }
                auto& material = model.materials[materialIndex];
                if (material.values.find("baseColorFactor") != material.values.end())
                {
                    auto color = material.values["baseColorFactor"].ColorFactor();
                    meshComponent.addColor = glm::vec4(color[0], color[1], color[2], color[3]);
                }
            }
        }
    }
    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (auto& sceneNodeIndex : scene.nodes)
    {
        entt::entity childEntity = nodeIndexToEntity[sceneNodeIndex];
        auto& childHierarchyComponent = registry.get<HierarchyComponent>(childEntity);
        childHierarchyComponent.parent = rootEntity;
        rootHierarchyComponent.children.push_back(childEntity);
    }

    nodeIndex = -1;
    for (auto& node : model.nodes)
    {
        nodeIndex++;
        if (!node.children.empty())
        {
            entt::entity parentEntity = nodeIndexToEntity[nodeIndex];
            auto& parentHierarchyComponent = registry.get<HierarchyComponent>(parentEntity);
            for (auto& childNode : node.children)
            {
                entt::entity childEntity = nodeIndexToEntity[childNode];
                auto& childHierarchyComponent = registry.get<HierarchyComponent>(childEntity);
                childHierarchyComponent.parent = parentEntity;
                parentHierarchyComponent.children.push_back(childEntity);
            }
        }
    }
    return rootEntity;
}

entt::entity bee::GLTFLoader::Duplicate(const entt::entity& entityToDuplicate, entt::entity rootEntity) const
{
    auto& registry = Engine.ECS().Registry();
    if (rootEntity == entt::null)
    {
        rootEntity = registry.create();
    }
    registry.emplace<RootComponent>(rootEntity);
    registry.emplace<TransformComponent>(rootEntity, registry.get<TransformComponent>(entityToDuplicate));
    registry.emplace<NameComponent>(rootEntity, registry.get<NameComponent>(entityToDuplicate));
    registry.emplace<HierarchyComponent>(rootEntity);

    CreateChildrenForDuplication(rootEntity, entityToDuplicate);

    return rootEntity;
}

void bee::GLTFLoader::CreateChildrenForDuplication(const entt::entity& parentToCopyIn, const entt::entity& parentOriginal) const
{
    auto& registry = Engine.ECS().Registry();
    auto& parentToCopyInHierarchyComponent = registry.get<HierarchyComponent>(parentToCopyIn);
    const auto& parentOriginalHierarchyComponent = registry.get<HierarchyComponent>(parentOriginal);
    for (auto childOriginal : parentOriginalHierarchyComponent.children)
    {
        entt::entity childToCopyIn = registry.create();
        parentToCopyInHierarchyComponent.children.push_back(childToCopyIn);
        registry.emplace<TransformComponent>(childToCopyIn, registry.get<TransformComponent>(childOriginal));
        registry.emplace<NameComponent>(childToCopyIn, registry.get<NameComponent>(childOriginal));
        if (const auto meshComponent = registry.try_get<MeshComponent>(childOriginal))
        {
            registry.emplace<MeshComponent>(childToCopyIn, *meshComponent);
        }
        auto& hierarchyComponent = registry.emplace<HierarchyComponent>(childToCopyIn);
        hierarchyComponent.parent = parentToCopyIn;

        CreateChildrenForDuplication(childToCopyIn, childOriginal);
    }
}
