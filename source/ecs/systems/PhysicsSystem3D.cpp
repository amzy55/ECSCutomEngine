#include "ecs/systems/PhysicsSystem3D.h"

#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "resource_managers/ResourceManager.h"
#include "tools/Hierarchy.h"

using namespace JPH;

bee::PhysicsSystem3D::PhysicsSystem3D(entt::registry& registry) : BaseSystem(registry)
{
    // Register allocation hook
    RegisterDefaultAllocator();

    // Create a factory
    Factory::sInstance = new Factory();

    // Register all Jolt physics types
    RegisterTypes();

    // preallocate 10MB
    m_tempAllocator = new TempAllocatorImpl(10 * 1024 * 1024);

    // job system
    m_jobSystem =
        new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, static_cast<int>(thread::hardware_concurrency() - 1));

    m_physicsSystem = new PhysicsSystem();
    // Initialize the actual physics system
    m_physicsSystem->Init(
        m_cMaxBodies, m_cNumBodyMutexes, m_cMaxBodyPairs, m_cMaxContactConstraints, m_broadPhaseLayerInterface,
        m_objectVsBroadphaseLayerFilter, m_objectVsObjectLayerFilter);

    // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection
    // performance (it's pointless here because we only have 2 bodies). You should definitely not call this every frame or when
    // e.g. streaming in a new level section as it is an expensive operation. Instead insert all new objects in batches instead
    // of 1 at a time to keep the broad phase efficient.
    //m_physicsSystem->OptimizeBroadPhase();
}

bee::PhysicsSystem3D::~PhysicsSystem3D()
{
    // Un-register all types with the factory and cleans up the default material
    UnregisterTypes();

    // Destroy the factory
    delete Factory::sInstance;
    Factory::sInstance = nullptr;

    delete m_physicsSystem;
    delete m_jobSystem;
    delete m_tempAllocator;
}

void bee::PhysicsSystem3D::Update(const float& dt)
{
    // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the
    // simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
    const int collisionSteps = dt <= 1.0f / 60.0f ? 1 : 2; // this keeps the simulation stable until 30 FPS
    m_physicsSystem->Update(dt, collisionSteps, m_tempAllocator, m_jobSystem);
}