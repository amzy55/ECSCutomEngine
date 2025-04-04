#include "rendering/debug_render.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tools/log.hpp"
#include "xsr/include/xsr.hpp"

using namespace bee;
using namespace glm;

bee::DebugRenderer::DebugRenderer()
{
    m_categoryFlags = DebugCategory::General | DebugCategory::Gameplay | DebugCategory::Physics | DebugCategory::Rendering |
                      DebugCategory::AINavigation | DebugCategory::AIDecision | DebugCategory::Editor;
}

DebugRenderer::~DebugRenderer() {}

void DebugRenderer::AddLine(DebugCategory::Enum category, const vec3& from, const vec3& to, const vec4& color)
{
    if (!(m_categoryFlags & category))
        return;

    xsr::render_debug_line(value_ptr(from), value_ptr(to), value_ptr(color));
}

void DebugRenderer::AddLine(DebugCategory::Enum category, const vec2& from, const vec2& to, const vec4& color)
{
    if (!(m_categoryFlags & category))
        return;

    AddLine(category, vec3(from.x, from.y, 0.0f), vec3(to.x, to.y, 0.0f), color);
}

void DebugRenderer::AddCircle(
    DebugCategory::Enum category, const vec3& center, float radius, const vec4& color, const Plane::Enum plane)
{
    if (!(m_categoryFlags & category))
        return;

    constexpr float dt = glm::two_pi<float>() / 64.0f;
    float t = 0.0f;

    vec3 v0;

    switch (plane)
    {
        case Plane::XY:
            v0 = {center.x + radius * cos(t), center.y + radius * sin(t), center.z};
            for (; t < glm::two_pi<float>() - dt; t += dt)
            {
                vec3 v1(center.x + radius * cos(t + dt), center.y + radius * sin(t + dt), center.z);
                AddLine(category, v0, v1, color);
                v0 = v1;
            }
            break;
        case Plane::XZ:
            v0 = {center.x + radius * cos(t), center.y, center.z + radius * sin(t)};
            for (; t < glm::two_pi<float>() - dt; t += dt)
            {
                vec3 v1(center.x + radius * cos(t + dt), center.y, center.z + radius * sin(t + dt));
                AddLine(category, v0, v1, color);
                v0 = v1;
            }
            break;
        case Plane::YZ:
            v0 = {center.x, center.y + radius * cos(t), center.z + radius * sin(t)};
            for (; t < glm::two_pi<float>() - dt; t += dt)
            {
                vec3 v1(center.x, center.y + radius * cos(t + dt), center.z + radius * sin(t + dt));
                AddLine(category, v0, v1, color);
                v0 = v1;
            }
            break;
    }
}

void DebugRenderer::AddSphere(DebugCategory::Enum category, const glm::vec3& center, float radius, const glm::vec4& color)
{
    if (!(m_categoryFlags & category))
        return;

    constexpr float dt = glm::two_pi<float>() / 64.0f;
    float t = 0.0f;
    vec3 v0;

    // xy
    v0 = {center.x + radius * cos(t), center.y + radius * sin(t), center.z};
    for (; t < glm::two_pi<float>() - dt; t += dt)
    {
        vec3 v1(center.x + radius * cos(t + dt), center.y + radius * sin(t + dt), center.z);
        AddLine(category, v0, v1, color);
        v0 = v1;
    }
    // xz
    t = 0.0f;
    v0 = {center.x + radius * cos(t), center.y, center.z + radius * sin(t)};
    for (; t < glm::two_pi<float>() - dt; t += dt)
    {
        vec3 v1(center.x + radius * cos(t + dt), center.y, center.z + radius * sin(t + dt));
        AddLine(category, v0, v1, color);
        v0 = v1;
    }
    // yz
    t = 0.0f;
    v0 = {center.x, center.y + radius * cos(t), center.z + radius * sin(t)};
    for (; t < glm::two_pi<float>() - dt; t += dt)
    {
        vec3 v1(center.x, center.y + radius * cos(t + dt), center.z + radius * sin(t + dt));
        AddLine(category, v0, v1, color);
        v0 = v1;
    }
}

void DebugRenderer::AddSquare(DebugCategory::Enum category, const glm::vec3& center, float size, const glm::vec4& color)
{
    if (!(m_categoryFlags & category))
        return;

    const float s = size * 0.5f;
    auto A = center + vec3(-s, -s, 0.0f);
    auto B = center + vec3(-s, s, 0.0f);
    auto C = center + vec3(s, s, 0.0f);
    auto D = center + vec3(s, -s, 0.0f);

    AddLine(category, A, B, color);
    AddLine(category, B, C, color);
    AddLine(category, C, D, color);
    AddLine(category, D, A, color);
}

void DebugRenderer::Render()
{
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.f / 1080.f, 0.1f, 100.0f);

    xsr::render(value_ptr(view), value_ptr(projection));
}
