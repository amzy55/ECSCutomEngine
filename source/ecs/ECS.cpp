#include "ecs/ECS.h"
#include "ecs/systems/AllSystemsInclude.h" // include new systems to this file
#include <glm/gtc/type_ptr.hpp>
#include "core/engine.hpp"
#include "ecs/components/TransformComponent.h"
#include "rendering/simple_renderer.hpp"

namespace bee
{
ECS::ECS()
{
    m_systems.emplace_back(std::make_unique<ParticleSystem>(m_registry));
    m_systems.emplace_back(std::make_unique<ScriptSystem>(m_registry));
    m_systems.emplace_back(std::make_unique<AbilitySystem>(m_registry));
    m_systems.emplace_back(std::make_unique<PhysicsSystem2D>(m_registry));
}

void ECS::Update(const float& dt) const
{
    for (auto& system : m_systems)
    {
        system->Update(dt);
    }
}

void ECS::Render() const
{
    for (auto& system : m_systems)
    {
        system->Render();
    }

    // render everything that has a matrix render component and mesh component
    const auto renderView = m_registry.view<TransformComponent, MeshComponent>();
    for (auto [entity, transformComponent, meshComponent] : renderView.each())
    {
        xsr::render_mesh(
            value_ptr(transformComponent.worldTransform), meshComponent.mesh->handle, meshComponent.texture->handle,
            value_ptr(meshComponent.mulColor), value_ptr(meshComponent.addColor));
    }

    Engine.SimpleRenderer().Render();
}

entt::registry& ECS::Registry() { return m_registry; }

} // namespace bee