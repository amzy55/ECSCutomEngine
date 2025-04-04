#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "core/ecs.hpp"
#include "platform/opengl/shader_gl.hpp"
#include "rendering/render_components.hpp"

namespace bee
{

struct CameraUBO;
struct PointLightsUBO;
struct TransformsUBO;
struct DirectionalLightsUBO;

struct DebugData
{
    bool BaseColor = false;
    bool Normals = false;
    bool NormalMap = false;
    bool Metallic = false;
    bool Roughness = false;
    bool Emissive = false;
    bool Occlusion = false;
};

class Renderer : public System
{
public:
    Renderer();
    ~Renderer() override;
    void Render() override;
    void LoadEnvironment(const std::string& filename);

private:
    static void SortMeshRenderers(entt::registry& reg, entt::entity ent);
    void RenderCurrentInstances(int instances);
    void CreateFrameBuffers();
    void DeleteFrameBuffers();
    void CreateShadowMaps();
    void DeleteShadowMaps();
    void RenderShadowMaps();
    void ApplyMaterial(std::shared_ptr<bee::Material> material);

    int m_width = -1;
    int m_height = -1;
    int m_msaa = 4;
    static const int m_max_dir_lights = 4; // In sync with Uniformsl.glsl
    const int m_shadowResolution = 2048;

    unsigned int m_msaaFramebuffer = 0;
    unsigned int m_msaaColorbuffer = 0;
    unsigned int m_msaaDepthbuffer = 0;
    unsigned int m_resolvedFramebuffer = 0;
    unsigned int m_resolvedColorbuffer = 0;
    unsigned int m_resolvedDepthbuffer = 0;
    unsigned int m_shadowFBOs[m_max_dir_lights];
    unsigned int m_shadowMaps[m_max_dir_lights];

    std::shared_ptr<Shader> m_toneMappingPass = nullptr;
    std::shared_ptr<Shader> m_envDebugPass = nullptr;

    DirectionalLightsUBO* m_dirLightsData = nullptr;
    unsigned int m_dirLightsUBO = 4294967295;

    CameraUBO* m_cameraData = nullptr;
    unsigned int m_cameraUBO = 4294967295;

    PointLightsUBO* m_lightsData = nullptr;
    unsigned int m_lightsUBO = 4294967295;

    TransformsUBO* m_transformsData = nullptr;
    unsigned int m_transformsUBO = 4294967295;

    std::shared_ptr<Material> m_currentMaterial;
    std::shared_ptr<Mesh> m_currentMesh;

    std::shared_ptr<Shader> m_forwardPass = nullptr;
    std::shared_ptr<Shader> m_post = nullptr;
    std::shared_ptr<Shader> m_shadowPass = nullptr;

    int m_materialChanges = -1;
    int m_meshChanges = -1;

    unsigned int m_hdrTexture = 0;
    unsigned int m_envCubemap = 0;
    unsigned int m_diffuseIBL = 0;
    unsigned int m_specularIBL = 0;
    unsigned int m_lutIBL = 0;
    DebugData m_debugData;
    int m_iblSpecularMipCount = -1;

#ifdef BEE_INSPECTOR
    void Reload();
    void Inspect() override;
    void Inspect(bee::Entity e) override;
    bool m_reload = false;
#endif
};

} // namespace bee
