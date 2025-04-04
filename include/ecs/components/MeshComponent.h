#pragma once
#include "rendering/simple_renderer.hpp"

namespace bee
{

struct MeshComponent
{
    std::shared_ptr<SimpleMesh> mesh;
    std::shared_ptr<SimpleTexture> texture;

    glm::vec4 mulColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 addColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
};

}
