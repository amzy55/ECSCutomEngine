#pragma once
#include <memory>
#include <vector>

#include "platform/opengl/uniforms_gl.hpp"

namespace bee
{
class SimpleRenderer;
struct SimpleMeshRender;
class DebugRenderer;
}  // namespace bee

struct Particle
{
    bool active = false;
    vec3 pos = vec3(0.f);
    vec3 velocity = vec3(0.f, 1.f, 0.f) * 5.f;  // dir + speed
    vec4 color = vec4(1.f);
    float size = 0.5f;
    float rotation = 45.f;  // value between 0 and 90 to give a random rotation
    float lifeSpan = 2.f;
};

class Emitter
{
public:
    Emitter(bee::SimpleRenderer* simpleRenderer);
    ~Emitter() = default;

    void AddParticle();
    void UpdateOneStep(float const dt);
    void UpdateAtFixedTS(float const dt);
    void ImGuiWindow();
    void Update(float const dt);
    void Render(bee::SimpleRenderer* simpleRenderer);

protected:
private:
    std::unique_ptr<bee::SimpleMeshRender> cube;
    // pool
    std::vector<Particle> m_particlePool;
    int m_poolSize = 40000;

    float m_accumulator = 0.f;
    float m_timeStep = 1.f / 30.f;
    float m_timeSinceLastSpawn = 0.f;
    
    int m_activeParticles = 0;
    int m_maxParticles = m_poolSize;

    struct EmitterProperties
    {
        bool fixedTimeStep = false;
        bool randomRotations = false;
        bool hasGravity = true;
        float gravity = 1.f;
        float particlesPerSecond = 18000.f;
        vec3 pos = vec3(0.f);
        vec3 dir = vec3(0.f, 1.f, 0.f);
        float speed = 2.f;
        vec4 colorBegin = vec4(0.f, 1.f, 1.f, 1.f);
        vec4 colorEnd = vec4(1.f, 0.f, 1.f, 1.f);
        float size = 0.5f;
        float lifeSpan = 2.f;
    } m_props;
};