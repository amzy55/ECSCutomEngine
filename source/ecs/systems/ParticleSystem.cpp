#include "ecs/systems/ParticleSystem.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.inl>
#include <glm/gtx/compatibility.hpp>

#include "tools/tools.hpp"
#include "core/input.hpp"
#include "core/engine.hpp"
#include "resource_managers/ResourceManager.h"
#include "tools/MainMenuBar.h"

#include "ecs/components/ParticleSystemComponents.h"
#include "ecs/components/TransformComponent.h"

namespace bee
{
ParticleSystem::ParticleSystem(entt::registry& registry) : BaseSystem(registry)
{
    m_mesh.mesh = Engine.ResourceManager().GetMeshManager().Get("cube").value();
    m_mesh.texture = Engine.ResourceManager().GetTextureManager().Get("white").value();
}

void ParticleSystem::CreateEmitter(const MeshComponent& mesh) const
{
    const entt::entity emitter = m_registry.create();
    m_registry.emplace<EmitterComponent>(emitter);
    auto& transformComponent = m_registry.emplace<TransformComponent>(emitter);
    transformComponent.scale = transformComponent.scale * 0.05f;
    m_registry.emplace<ParticleConfigComponent>(emitter);
    //auto& meshComponent = m_registry.emplace<MeshComponent>(emitter);
    //meshComponent.mesh = mesh.mesh;
    //meshComponent.texture = mesh.texture;
}

void ParticleSystem::AddParticle(const entt::entity& emitter) const
{
    const entt::entity particle = m_registry.create();

    auto& particleComponent = m_registry.emplace<ParticleComponent>(particle);
    m_registry.emplace<EmptyParticleComponent>(particle);
    const auto& particleConfigComponent = m_registry.get<ParticleConfigComponent>(emitter);
    particleComponent.dir = glm::normalize(
        particleConfigComponent.dir + glm::vec3(bee::RandomFloatBtwnMinus1and1(), 0.f, bee::RandomFloatBtwnMinus1and1()));
    particleComponent.gravity = particleConfigComponent.gravity;
    particleComponent.speed = particleConfigComponent.speed;
    particleComponent.colorBegin = particleConfigComponent.colorBegin;
    particleComponent.colorEnd = particleConfigComponent.colorEnd;
    particleComponent.scaleBegin = particleConfigComponent.scaleBegin;
    particleComponent.scaleEnd = particleConfigComponent.scaleEnd;
    particleComponent.lifeSpan = particleConfigComponent.lifeSpan;
    particleComponent.currentLife = particleConfigComponent.lifeSpan;

    auto& transformComponent = m_registry.emplace<TransformComponent>(particle);
    const auto& emitterTransformComponent = m_registry.get<TransformComponent>(emitter);
    transformComponent.pos += emitterTransformComponent.pos;

    particleComponent.dir = emitterTransformComponent.rotation * particleComponent.dir;
    particleComponent.dir = normalize(particleComponent.dir);
    // transformComponent.rotation *= emitterTransformComponent.rotation;
    // transformComponent.rotation = normalize(transformComponent.rotation);
    transformComponent.scale *= emitterTransformComponent.scale;
    particleComponent.scaleBegin = transformComponent.scale;

    auto& meshComponent = m_registry.emplace<MeshComponent>(particle, m_mesh/*m_registry.get<MeshComponent>(emitter)*/);
    meshComponent.addColor = particleComponent.colorBegin;
}

void ParticleSystem::UpdateOneStep(const float& dt) const
{
    const auto particleView = m_registry.view<ParticleComponent, TransformComponent, MeshComponent>();
    for (auto [particle, particleComponent, transformComponent, meshComponent] : particleView.each())
    {
        particleComponent.currentLife -= dt;
        if (particleComponent.currentLife >= 0)
        {
            particleComponent.dir.y -= particleComponent.gravity * dt;
            // particleComponent.dir = normalize(particleComponent.dir);
            transformComponent.pos += particleComponent.dir * particleComponent.speed * dt;
            const float& lerpStep = particleComponent.currentLife / particleComponent.lifeSpan;
            meshComponent.addColor = glm::lerp(particleComponent.colorEnd, particleComponent.colorBegin, lerpStep);
            transformComponent.scale = glm::lerp(particleComponent.scaleEnd, particleComponent.scaleBegin, lerpStep);
        }
        else
        {
            m_registry.destroy(particle);
        }
    }

    const auto emitterView = m_registry.view<EmitterComponent>();
    for (auto [emitter, emitterComponent] : emitterView.each())
    {
        if (emitterComponent.endless == false)
            emitterComponent.currentLife -= dt;
        if (emitterComponent.currentLife >= 0 || emitterComponent.endless)
        {
            emitterComponent.timeSinceLastSpawn += dt;
            int const particlesToSpawn =
                static_cast<int>(emitterComponent.timeSinceLastSpawn * emitterComponent.particlesPerSecond);
            if (particlesToSpawn != 0)
                emitterComponent.timeSinceLastSpawn -=
                    static_cast<float>(particlesToSpawn) / emitterComponent.particlesPerSecond;

            for (int i = 0; i < particlesToSpawn; i++)
            {
                AddParticle(emitter);
            }
        }
        else
        {
            m_registry.destroy(emitter);
        }
    }
}

void ParticleSystem::UpdateAtFixedTS(const float& dt)
{
    m_accumulator += dt;
    if (m_accumulator >= m_timeStep)
    {
        while (m_accumulator > dt)
        {
            UpdateOneStep(m_timeStep);
            m_accumulator -= m_timeStep;
        }
    }
}

void ParticleSystem::ImGuiWindow()
{
    if (!Engine.MainMenuBar().IsFlagOn(WindowsToDisplay::EmitterMenu))
        return;

    const auto emitterView = m_registry.view<EmitterComponent>();
    const size_t emitterCountSizeT = emitterView.size();
    const int emitterCountInt = static_cast<int>(emitterCountSizeT);
    std::vector<std::string> emitterIDString;
    std::vector<unsigned> emitterIDInt;
    for (auto& emitter : emitterView)
    {
        emitterIDString.push_back(std::to_string(entt::to_integral(emitter)));
        emitterIDInt.push_back(entt::to_integral(emitter));
    }

    ImGui::SetNextWindowSize({350.f, 300.f}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({0.f, 650.f}, ImGuiCond_Once);
    ImGui::Begin("Emitter List");
    bool toCreate = false;
    if (ImGui::Button("Create", {50.f, 20.f}))
        toCreate = true;
    ImGui::Text("Emitter count: %d / %d", emitterCountInt, m_maxEmitters);

    for (int i = 0; i < emitterCountInt; i++)
    {
        const bool isSelected = (m_selectedEmitter == i);
        if (ImGui::Selectable(emitterIDString[i].c_str(), isSelected))
        {
            m_selectedEmitter = i;
        }
    }
    ImGui::End();

    if (m_selectedEmitter >= 0 && m_selectedEmitter < emitterCountInt)
    {
        ImGui::SetNextWindowSize({350.f, 400.f}, ImGuiCond_Once);
        ImGui::SetNextWindowPos({1920.f - 350.f, 350.f}, ImGuiCond_Once); // hardcoded values for a 1920x1080 window
        ImGui::Begin("Emitter Editor");
        bool toDelete = false;
        const auto emitter = static_cast<entt::entity>(emitterIDInt[m_selectedEmitter]);

        if (ImGui::CollapsingHeader("Emitter properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Emitter ID: %d", emitterIDInt[m_selectedEmitter]);

            // Set button colors
            ImGui::PushStyleColor(ImGuiCol_Button, {0.65f, 0.15f, 0.15f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.8f, 0.3f, 0.3f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1.f, 0.2f, 0.2f, 1.f});
            if (ImGui::Button("Destroy", {60.f, 20.f}))
                toDelete = true;
            ImGui::PopStyleColor(3);
            auto& emitterComponent = m_registry.get<EmitterComponent>(emitter);
            auto& particleConfigComponent = m_registry.get<ParticleConfigComponent>(emitter);

            ImGui::Checkbox("Endless", &emitterComponent.endless);
            if (emitterComponent.endless == false)
            {
                ImGui::SliderFloat("Emitter life left", &emitterComponent.currentLife, 0.f, emitterComponent.lifeSpan);
                ImGui::SliderFloat("Emitter life span", &emitterComponent.lifeSpan, 0.f, 100.f);
                if (emitterComponent.lifeSpan < emitterComponent.currentLife)
                    emitterComponent.currentLife = emitterComponent.lifeSpan;
            }
            ImGui::SliderFloat("Emit rate", &emitterComponent.particlesPerSecond, 0.f, 1000.f);
            ImGui::DragFloat("Gravity", &(particleConfigComponent.gravity));
            ImGui::ColorEdit4("Start color", glm::value_ptr(particleConfigComponent.colorBegin));
            ImGui::ColorEdit4("End color", glm::value_ptr(particleConfigComponent.colorEnd));
            ImGui::DragFloat("Speed", &(particleConfigComponent.speed));
            ImGui::DragFloat("Particle life", &(particleConfigComponent.lifeSpan));

        }
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& transformComponent = m_registry.get<TransformComponent>(emitter);
            ImGui::DragFloat3("Position", value_ptr(transformComponent.pos));
            glm::vec3 rotation = transformComponent.GetRotationInDegrees();
            ImGui::DragFloat3("Rotation", value_ptr(rotation));
            transformComponent.rotation = GetQuatFromDegrees(rotation);
            ImGui::DragFloat3("Scale", value_ptr(transformComponent.scale));
        }
        if (toDelete)
            m_registry.destroy(emitter);
        ImGui::End();
    }
    
    // this is not placed appropriately
    if (toCreate && emitterCountInt < m_maxEmitters)
    {
        CreateEmitter(m_mesh);
    }
    if (m_selectedEmitter == -1)
        m_selectedEmitter = 0;
}

void ParticleSystem::Update(const float& dt)
{
    if (m_fixedTimeStep == false)
        UpdateOneStep(dt);
    else
        UpdateAtFixedTS(dt);

    ImGuiWindow();
}

void ParticleSystem::Render() const
{
    const auto renderView = m_registry.view<TransformComponent, MeshComponent, EmptyParticleComponent>();
    for (auto [entity, transformComponent, meshComponent] : renderView.each())
    {
        xsr::render_mesh(
            value_ptr(transformComponent.GetTransformMatrix()), meshComponent.mesh->handle, meshComponent.texture->handle,
            value_ptr(meshComponent.mulColor), value_ptr(meshComponent.addColor));
    }
}
} // namespace bee