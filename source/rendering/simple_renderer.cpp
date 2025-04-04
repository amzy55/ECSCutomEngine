#include "rendering/simple_renderer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "core/engine.hpp"
#include "core/fileio.hpp"
#include "ecs/ECS.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/ScriptComponent.h"
#include "tools/log.hpp"

using namespace bee;
using namespace glm;

SimpleMesh::SimpleMesh(const std::string& path)
{
    auto data = Engine.FileIO().ReadTextFile(FileIO::Directory::Asset, path);
    if (!data.empty())
        handle = xsr::tools::load_obj_mesh(data);
}

bee::SimpleMesh::SimpleMesh(const std::string& path, const std::string& object)
{
    auto data = Engine.FileIO().ReadTextFile(FileIO::Directory::Asset, path);
    if (!data.empty())
        handle = xsr::tools::load_obj_mesh(data, object);
}

bee::SimpleMesh::SimpleMesh(
    const std::vector<unsigned int>& indices, const std::vector<vec3>& positions, const std::vector<vec3>& normals,
    const std::vector<vec2>& texture_coordinates, const std::vector<vec3>& colors)
{
    auto* normals_data = normals.empty() ? nullptr : &normals.data()->x;
    auto* texture_data = texture_coordinates.empty() ? nullptr : &texture_coordinates.data()->x;
    auto* colors_data = colors.empty() ? nullptr : &colors.data()->x;
    handle = xsr::create_mesh(
        indices.data(), static_cast<unsigned>(indices.size()), &positions.data()->x, normals_data, texture_data, colors_data,
        static_cast<unsigned>(positions.size()));
}

SimpleMesh::SimpleMesh(const xsr::mesh_handle& handle) { this->handle = handle; }

SimpleTexture::SimpleTexture(const std::string& path)
{
    auto data = Engine.FileIO().ReadBinaryFile(FileIO::Directory::Asset, path);
    if (!data.empty())
        handle = xsr::tools::load_png_texture(data);
}

SimpleTexture::SimpleTexture(const xsr::texture_handle& handle) { this->handle = handle; }

bee::SimpleRenderer::SimpleRenderer()
{
    // create xsr render config
    xsr::render_configuration config;
    config.enable_shadows = true;
    config.shadow_resolution = 1024;
    config.texture_filter = xsr::render_configuration::texture_filtering::linear;
    config.shader_path = "external/xsr/";
    const auto success = xsr::initialize(config);
    if (!success)
    {
        Log::Error("Failed to initialize xsr!\a");
    }

    // light
    for (auto i = 0; i < 2; i++) //  max 4 lights
    {
        m_lights.emplace_back();
        m_lights.back().color = vec3(1.f);

        switch (i)
        {
            case 0:
                m_lights.back().direction = normalize(vec3(-1.f, -1.f, -1.f));
                break;
            case 1:
                m_lights.back().direction = normalize(vec3(1.f, -0.5f, 0.f));
                break;
            case 2:
                m_lights.back().direction = normalize(vec3(1.f, -1.f, -1.f));
                break;
            case 3:
                m_lights.back().direction = normalize(vec3(-1.f, 1.f, 1.f));
                break;
            default:
                m_lights.back().direction = vec3(0.f, -1.f, 0.f);
                break;
        }
    }
}

bee::SimpleRenderer::~SimpleRenderer() { xsr::shutdown(); }

void bee::SimpleRenderer::Render()
{
    for (auto& light : m_lights)
    {
        xsr::render_directional_light(value_ptr(light.direction), value_ptr(light.color));
    }

    auto& registry = Engine.ECS().Registry();
    const auto cameraView = registry.view<CameraComponent>();
    auto& cameraComponent = Engine.ECS().Registry().get<CameraComponent>(cameraView.front());
    xsr::render(value_ptr(cameraComponent.view), value_ptr(cameraComponent.projection));
}

void bee::SimpleRenderer::RenderMesh(SimpleMeshRender* mesh, mat4 transform, vec4 addColor)
{
    xsr::render_mesh(
        value_ptr(transform), mesh->mesh->handle, mesh->texture->handle, value_ptr(mesh->mulColor), value_ptr(addColor));
}

void bee::SimpleRenderer::InitCamera()
{
    auto& registry = Engine.ECS().Registry();
    const auto cameraView = registry.view<CameraComponent>();
    if (cameraView.front() == entt::null)
    {
        auto camera = Engine.ECS().Registry().create();
        auto& cameraComponent = Engine.ECS().Registry().emplace<CameraComponent>(camera);
        cameraComponent.view = glm::rotate(cameraComponent.view, glm::radians(60.0f), {1.f, 0.f, 0.f});
        cameraComponent.view = glm::translate(cameraComponent.view, vec3(0.0f, -75.f, -40.0f));
        cameraComponent.projection = glm::perspective(glm::radians(45.0f), 1920.f / 1080.f, 0.1f, 1000.0f);
        auto& scriptComponent = Engine.ECS().Registry().emplace<ScriptComponent>(camera);
        scriptComponent.scripts.emplace_back("assets/scripts/camera_script.lua");
    }
}
