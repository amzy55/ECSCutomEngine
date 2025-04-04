#pragma once
#include "BaseSystem.h"
#include "ecs/components/MeshComponent.h"

namespace bee
{

class ParticleSystem final : public BaseSystem
{
public:
    explicit ParticleSystem(entt::registry& registry);

    void Update(const float& dt) override;
    void Render() const override;

private:
    void CreateEmitter(const MeshComponent& mesh) const;
    void AddParticle(const entt::entity& emitter) const;
    void UpdateOneStep(const float& dt) const;
    void UpdateAtFixedTS(const float& dt);
    void ImGuiWindow();

    MeshComponent m_mesh;
    const int m_maxEmitters = 10;
    int m_selectedEmitter = -1;

    bool m_fixedTimeStep = false;
    float m_timeStep = 1.f / 30.f;
    float m_accumulator = 0.f;
};

} // namespace bee