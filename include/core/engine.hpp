#pragma once
#include <memory>
#include <string>

namespace bee
{
class FileIO;
class Device;
class Input;
class DebugRenderer;
class SimpleRenderer;
class ECS;
class ResourceManager;
class Hierarchy;
class Serializer;
class GLTFLoader;
class Scene;
class MainMenuBar;

class EngineClass
{
public:
    void Initialize();
    void Shutdown();
    void Run();

    [[nodiscard]] FileIO& FileIO() const { return *m_fileIO; }
    [[nodiscard]] Device& Device() const { return *m_device; }
    [[nodiscard]] Input& Input() const { return *m_input; }
    [[nodiscard]] ResourceManager& ResourceManager() const { return *m_resourceManager; }
    [[nodiscard]] ECS& ECS() const { return *m_ecs; }
    [[nodiscard]] DebugRenderer& DebugRenderer() const { return *m_debugRenderer; }
    [[nodiscard]] SimpleRenderer& SimpleRenderer() const { return *m_simpleRenderer; }
    [[nodiscard]] Hierarchy& Hierarchy() const { return *m_hierarchy; }
    [[nodiscard]] Serializer& Serializer() const { return *m_serializer; }
    [[nodiscard]] GLTFLoader& GLTFLoader() const { return *m_gltfLoader; }
    [[nodiscard]] MainMenuBar& MainMenuBar() const { return *m_mainMenuBar; }

private:
    std::unique_ptr<bee::FileIO> m_fileIO = nullptr;
    std::unique_ptr<bee::Device> m_device = nullptr;
    std::unique_ptr<bee::Input> m_input = nullptr;
    std::unique_ptr<bee::ResourceManager> m_resourceManager = nullptr;
    std::unique_ptr<bee::ECS> m_ecs = nullptr;
    std::unique_ptr<bee::DebugRenderer> m_debugRenderer = nullptr;
    std::unique_ptr<bee::SimpleRenderer> m_simpleRenderer = nullptr;
    std::unique_ptr<bee::Hierarchy> m_hierarchy = nullptr;
    std::unique_ptr<bee::Serializer> m_serializer = nullptr;
    std::unique_ptr<bee::GLTFLoader> m_gltfLoader = nullptr;
    std::unique_ptr<bee::Scene> m_scene = nullptr;
    std::unique_ptr<bee::MainMenuBar> m_mainMenuBar = nullptr;
};

extern EngineClass Engine;

} // namespace bee