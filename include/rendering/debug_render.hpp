#pragma once

#include <glm/glm.hpp>

namespace bee
{

struct Plane // for drawing circles
{
    enum Enum
    {
        XY = 0,
        XZ,
        YZ
    };
};

struct DebugCategory
{
    enum Enum
    {
        General = 1 << 0,
        Gameplay = 1 << 1,
        Physics = 1 << 2,
        Sound = 1 << 3,
        Rendering = 1 << 4,
        AINavigation = 1 << 5,
        AIDecision = 1 << 6,
        Editor = 1 << 7,
        AccelStructs = 1 << 8,
        All = 0xFFFFFFFF
    };
};

class DebugRenderer
{
public:
    DebugRenderer();
    ~DebugRenderer();

    void AddLine(DebugCategory::Enum category, const glm::vec3& from, const glm::vec3& to, const glm::vec4& color);

    void AddLine(DebugCategory::Enum category, const glm::vec2& from, const glm::vec2& to, const glm::vec4& color);

    void AddCircle(
        DebugCategory::Enum category, const glm::vec3& center, float radius, const glm::vec4& color, Plane::Enum plane = Plane::XY);

    void AddSphere(DebugCategory::Enum category, const glm::vec3& center, float radius, const glm::vec4& color);

    void AddSquare(DebugCategory::Enum category, const glm::vec3& center, float size, const glm::vec4& color);

    void Render();

    void SetCategoryFlags(unsigned int flags) { m_categoryFlags = flags; }
    unsigned int GetCategoryFlags() const { return m_categoryFlags; }

private:
    unsigned int m_categoryFlags;
};

} // namespace bee
