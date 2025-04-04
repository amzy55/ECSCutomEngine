#pragma once
#include <entt/entity/registry.hpp>

namespace bee
{
class BaseSystem
{
public:
    // All new systems must be derived from the BasesSystem class
    explicit BaseSystem(entt::registry& registry);
    virtual ~BaseSystem() = default;
    BaseSystem(const BaseSystem&) = delete;
    BaseSystem& operator=(const BaseSystem&) = delete;
    BaseSystem(const BaseSystem&&) = delete;
    BaseSystem& operator=(const BaseSystem&&) = delete;

    /**
     * \brief The behavior of the system
     * \param dt delta time
     */
    virtual void Update(const float& dt) = 0;

    /**
     * \brief Implement if the system needs custom rendering.
     */
    virtual void Render() const {}

protected:
    entt::registry& m_registry;

private:

};

}  // namespace bee