#pragma once

#include "entt/entity/registry.hpp"

namespace bee
{
class PhysicsSystem3D;
class BaseSystem;

class ECS
{
public:
    ECS();

    /**
     * \brief Updates all systems.
     * \param dt delta time
     */
    void Update(const float& dt) const;

    /**
     * \brief Renders all entities that have MatrixRenderComponent and MeshComponent;
     * and calls the custom rendering of systems
     * that implemented the Render() method of BaseSystem.
     */
    void Render() const;

    // Makes the registry publicly accessible.
    entt::registry& Registry();

private:
    entt::registry m_registry;

    std::vector<std::unique_ptr<BaseSystem>> m_systems;
};

} // namespace bee
