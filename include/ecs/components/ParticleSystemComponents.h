#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace bee
{

struct ParticleComponent
{
    float gravity = 1.f;
    glm::vec3 dir = glm::vec3(0.f, 1.f, 0.f);
    float speed = 2.f;
    glm::vec4 colorBegin = glm::vec4(0.f, 1.f, 1.f, 1.f);
    glm::vec4 colorEnd = glm::vec4(1.f, 0.f, 1.f, 1.f);
    glm::vec3 scaleBegin = glm::vec3(1.f);
    glm::vec3 scaleEnd = glm::vec3(0.f);
    float lifeSpan = 2.f;
    float currentLife = lifeSpan;
};

struct EmptyParticleComponent
{
};

struct EmitterComponent
{
    float particlesPerSecond = 250.f;
    bool endless = true;
    float lifeSpan = 60.f;

    float currentLife = lifeSpan;
    float timeSinceLastSpawn = 0.f;
};

struct ParticleConfigComponent
{
    float gravity = 1.f;
    glm::vec3 dir = glm::vec3(0.f, 1.f, 0.f);
    float speed = 2.f;
    glm::vec4 colorBegin = glm::vec4(0.f, 1.f, 1.f, 1.f);
    glm::vec4 colorEnd = glm::vec4(1.f, 0.f, 1.f, 1.f);
    glm::vec3 scaleBegin = glm::vec3(1.f);
    glm::vec3 scaleEnd = glm::vec3(0.f);
    float lifeSpan = 2.f;
};

}
