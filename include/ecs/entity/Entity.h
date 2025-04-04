#pragma once
#include "entt/entity/registry.hpp"

namespace bee
{

class Entity
{
public:
    Entity() = default;
    explicit Entity(entt::entity entity);
    
    template<typename T>
    T& GetComponent();

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args);

    template<typename T>
    void RemoveComponent();

    explicit operator bool() const { return m_entity != entt::null; }
    explicit operator entt::entity() const { return m_entity; }
    explicit operator uint32_t() const { return static_cast<uint32_t>(m_entity); }

private:
	entt::entity m_entity{ entt::null };
};

}
