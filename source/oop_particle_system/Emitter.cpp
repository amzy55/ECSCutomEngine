#include "tools/tools.hpp"
#include "rendering/simple_renderer.hpp"
#include "oop_particle_system/Emitter.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include "platform/opengl/uniforms_gl.hpp"

Emitter::Emitter(bee::SimpleRenderer* simpleRenderer)
{
    m_particlePool.reserve(m_poolSize);
    const Particle defaultParticle;
    for (int i = 0; i < m_poolSize; i++)
    {
        m_particlePool.push_back(defaultParticle); // populating the pool
    }

    // mesh
    std::vector<unsigned int> indices = {// front
                                         0, 1, 2, 2, 3, 0,
                                         // right
                                         1, 5, 6, 6, 2, 1,
                                         // back
                                         7, 6, 5, 5, 4, 7,
                                         // left
                                         4, 0, 3, 3, 7, 4,
                                         // bottom
                                         4, 5, 1, 1, 0, 4,
                                         // top
                                         3, 2, 6, 6, 7, 3};
    // 8 elements
    std::vector<vec3> vertices = {vec3(-1.0, -1.0, 1.0),  vec3(1.0, -1.0, 1.0),  vec3(1.0, 1.0, 1.0),  vec3(-1.0, 1.0, 1.0),
                                  vec3(-1.0, -1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(1.0, 1.0, -1.0), vec3(-1.0, 1.0, -1.0)};
    std::vector<vec3> normals = {
        normalize(normalize(vec3(-1.0, -1.0, 1.0))),
        normalize(vec3(1.0, -1.0, 1.0)),
        normalize(vec3(1.0, 1.0, 1.0)),
        normalize(vec3(-1.0, 1.0, 1.0)),
        normalize(vec3(-1.0, -1.0, -1.0)),
        normalize(vec3(1.0, -1.0, -1.0)),
        normalize(vec3(1.0, 1.0, -1.0)),
        normalize(vec3(-1.0, 1.0, -1.0))};
    std::vector<vec2> texCoords = {vec2(0.f, 0.f), vec2(1.f, 0.f), vec2(0.f, 1.f), vec2(1.f, 1.f),
                                   vec2(0.f, 0.f), vec2(1.f, 0.f), vec2(0.f, 1.f), vec2(1.f, 1.f)};
    std::vector<vec3> colors = {// all white
                                vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0),
                                vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0),
                                vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)};
    cube = std::make_unique<bee::SimpleMeshRender>();
    cube->mesh = std::make_shared<bee::SimpleMesh>(indices, vertices, normals, texCoords, colors);
    cube->texture = std::make_shared<bee::SimpleTexture>("textures/white.png");
}

void Emitter::AddParticle()
{
    if (m_activeParticles < m_maxParticles)
    {
        for (Particle& particle : m_particlePool)
        {
            if (!particle.active)
            {
                particle.active = true;
                particle.pos = m_props.pos;
                vec3 const randDir =
                    normalize(m_props.dir + vec3(bee::RandomFloatBtwnMinus1and1(), 0.f, bee::RandomFloatBtwnMinus1and1()));
                particle.velocity = randDir * m_props.speed;
                particle.color = m_props.colorBegin;
                particle.size = m_props.size;
                particle.rotation = static_cast<float>(rand() % 90);
                particle.lifeSpan = m_props.lifeSpan;

                m_activeParticles++;

                break;
            }
        }
    }
}

void Emitter::UpdateOneStep(float const dt)
{
    for (Particle& particle : m_particlePool)
    {
        if (particle.active)
        {
            if (particle.lifeSpan >= 0)
            {
                if (m_props.hasGravity)
                    particle.velocity.y -= m_props.gravity * dt;
                particle.pos += particle.velocity * dt;
                float const deltaColor = particle.lifeSpan / m_props.lifeSpan;
                particle.color = glm::lerp(m_props.colorEnd, m_props.colorBegin, deltaColor);
                particle.size = glm::lerp(0.f, m_props.size, deltaColor);
                particle.lifeSpan -= dt;
            }
            else
            {
                m_activeParticles--;
                particle.active = false;
            }
        }
    }

    m_timeSinceLastSpawn += dt;
    int const particlesToSpawn = static_cast<int>(m_timeSinceLastSpawn * m_props.particlesPerSecond);
    if (particlesToSpawn != 0)
        m_timeSinceLastSpawn -= static_cast<float>(particlesToSpawn) / m_props.particlesPerSecond;

    for (int i = 0; i < particlesToSpawn; i++)
    {
        AddParticle();
    }
}

void Emitter::UpdateAtFixedTS(float const dt)
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

void Emitter::ImGuiWindow()
{
    // ImGUI window creation
    ImGui::Begin("Particle System");
    ImGui::SetWindowFontScale(1.5f);
    ImGui::ColorEdit4("Start color", glm::value_ptr(m_props.colorBegin));
    ImGui::ColorEdit4("End color", glm::value_ptr(m_props.colorEnd));
    ImGui::SliderFloat("Speed", &(m_props.speed), 0.0f, 5.0f);
    ImGui::SliderFloat("Size", &(m_props.size), 0.0f, 5.0f);
    ImGui::SliderFloat(
        "Emit rate", &(m_props.particlesPerSecond), 0.0f, static_cast<float>(m_maxParticles) / std::max(m_props.lifeSpan, 1.f));
    ImGui::SliderFloat("Particle lifetime", &(m_props.lifeSpan), 0.01f, 5.0f);
    ImGui::Checkbox("Fixed time step 30 FPS", &(m_props.fixedTimeStep));
    ImGui::Checkbox("Random rotations", &(m_props.randomRotations));
    ImGui::SliderFloat("Gravity", &(m_props.gravity), 0.0f, 9.8f);
    ImGui::Checkbox("Has gravity", &(m_props.hasGravity));

    ImGui::Text("Active particles: %d", (m_activeParticles));
    ImGui::Text("Max possible particles: %d", (m_maxParticles));
    ImGui::End();
}

void Emitter::Update(float const dt)
{
    if (!m_props.fixedTimeStep)
        UpdateOneStep(dt);
    else
        UpdateAtFixedTS(dt);

    ImGuiWindow();
}

void Emitter::Render(bee::SimpleRenderer* simpleRenderer)
{
    for (Particle& particle : m_particlePool)
    {
        if (particle.active)
        {
            mat4 transform = mat4(1.0f);
            transform = glm::translate(transform, particle.pos);
            transform = glm::scale(transform, vec3(0.035f) * particle.size);
            if (m_props.randomRotations)
                transform = glm::rotate(transform, glm::radians(particle.rotation), vec3(1.f, 1.f, 1.f));
            simpleRenderer->RenderMesh(cube.get(), transform, vec4(particle.color.x, particle.color.y, particle.color.z, 1.f));
        }
    }
    simpleRenderer->Render();
}
