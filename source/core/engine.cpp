#include "core/engine.hpp"

#include <imgui.h>
#include <imgui_impl.h>
#include <chrono>

#include "core/device.hpp"
#include "core/fileio.hpp"
#include "core/input.hpp"
#include "rendering/debug_render.hpp"
#include "rendering/simple_renderer.hpp"
#include "tools/log.hpp"
#include "resource_managers/ResourceManager.h"
#include "tools/FPSInfo.h"
#include "tools/GLTFLoader.h"
#include "tools/Hierarchy.h"
#include "ecs/ECS.h"
#include "tools/Serializer.h"
#include "ecs/systems/AllSystemsInclude.h"
#include "blockB/Scene.h"
#include "tools/MainMenuBar.h"

using namespace bee;

// Make the engine a global variable on free store memory.
bee::EngineClass bee::Engine;

void EngineClass::Initialize()
{
    Log::Initialize();

    m_fileIO = std::make_unique<bee::FileIO>();
    m_device = std::make_unique<bee::Device>();
    m_input = std::make_unique<bee::Input>();
    m_resourceManager = std::make_unique<bee::ResourceManager>();
    m_ecs = std::make_unique<bee::ECS>();
    m_debugRenderer = std::make_unique<bee::DebugRenderer>();
    m_simpleRenderer = std::make_unique<bee::SimpleRenderer>();
    ImGui_Impl_Init();
    m_hierarchy = std::make_unique<bee::Hierarchy>(ECS().Registry());
    m_serializer = std::make_unique<bee::Serializer>();
    m_gltfLoader = std::make_unique<bee::GLTFLoader>();
    m_scene = std::make_unique<bee::Scene>(m_ecs->Registry());
    m_mainMenuBar = std::make_unique<bee::MainMenuBar>();

    // serialization is dependent on the order of the model loading
    // if you want to change order you'll have to first delete the serialized.cereal file
    m_gltfLoader->Load("assets/models/Mecha.gltf");
    //m_gltfLoader->Load("assets/models/ruins.gltf");
    //m_gltfLoader->Load("assets/models/GearboxAssy.gltf");

    m_serializer->Deserialize();
    m_simpleRenderer->InitCamera();
    m_hierarchy->Init();

    if (m_serializer->IsThereACerealFile() == false)
    {
        //m_gltfLoader->ConstructOrDuplicate("assets/models/Mecha.gltf");
        //m_gltfLoader->ConstructOrDuplicate("assets/models/ruins.gltf");
        //m_gltfLoader->ConstructOrDuplicate("assets/models/GearboxAssy.gltf");
    }

    m_scene->Init();
}

void EngineClass::Shutdown()
{
    ImGui_Impl_Shutdown();
    m_serializer->Serialize();
}

void EngineClass::Run()
{
    auto time = std::chrono::high_resolution_clock::now();
    
    while (!m_device->ShouldClose())
    {
        ImGui_Impl_NewFrame();

        auto ctime = std::chrono::high_resolution_clock::now();
        auto elapsed = ctime - time;
        float dt = std::chrono::duration<float>(elapsed).count();
        
        m_input->Update();
        m_device->Update();
        m_mainMenuBar->Display(dt);
        PrintFPS(dt);
        m_resourceManager->ImGuiWindow();
        m_scene->Update();
        m_ecs->Update(dt);
        m_hierarchy->DisplayHierarchyAndCalculateTransforms();
        m_ecs->Render();

        time = ctime;

        ImGui_Impl_RenderDrawData();

        if (m_input->GetKeyboardKey(Input::KeyboardKey::Escape))
            m_device->CloseWindow();
    }
}
