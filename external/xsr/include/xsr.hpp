#pragma once

#include <vector>
#include <string>

namespace xsr
{

typedef unsigned char uchar;

/// <summary>
/// Handle for meshes and textures, used for type safety
/// </summary>
struct handle
{
    int id = 0;
    bool operator==(const handle& other) const { return id == other.id; }
    bool operator!=(const handle& other) const { return id != other.id; }
    bool is_valid() const { return id != 0; }
};

/// <summary>
/// Handle for meshes.
/// </summary>
struct mesh_handle : public handle {};

/// <summary>
/// Handle for textures.
/// </summary>
struct texture_handle : public handle {};

/// <summary>
/// Used to configure the device (window) on all platforms.
/// </summary>
struct device_configuration
{
	int width = 0;
	int height = 0;
	std::string title;
};

/// <summary>
/// Used to configure the renderer
/// </summary>
struct render_configuration
{
    enum class texture_filtering { nearest, linear };

    bool enable_shadows = true;
    int shadow_resolution = 1024;
    texture_filtering texture_filter = texture_filtering::linear;
    std::string shader_path = "../../";
};

/// <summary>
/// Initialized the renderer (buffers for rendering, shaders etc)
/// </summary>
bool initialize(const render_configuration& config);

/// <summary>
/// Shutdown the renderer (clear the buffers for rendering, shaders etc)
/// </summary>
void shutdown();

/// <summary>
/// Creates a mesh from vertex data.
/// Must provide all attributes and the same number of vertices for each attribute.
/// </summary>
/// <param name="indices">Index buffer</param>
/// <param name="index_count">Index count</param>
/// <param name="positions">Position buffer. Three elements per position.</param>
/// <param name="normals">Normal buffer. Should be normalized already. Three elements per normal.</param>
/// <param name="texture_coordinates">Texture coordinates. Three elements per normal.</param>
/// <param name="colors">Vertex colors. Three elements per color in [0,1] range.</param>
/// <param name="vertex_count">The number of vertices. This must the same for all attributes.</param>
/// <returns>A mesh handle</returns>
mesh_handle create_mesh(
    const unsigned int* indices,
    unsigned int index_count,
    const float* positions,
    const float* normals,
    const float* texture_coordinates,
    const float* colors,
    unsigned int vertex_count);

/// <summary>
/// Creates a texture from pixel data.
/// </summary>
texture_handle create_texture(
    int width,
    int height,
    const void* pixel_rgba_bytes);

/// <summary>
/// Renders a directional light.
/// </summary>
/// <param name="direction">The direction of the light. Should be normalized.</param>
/// <param name="color_and_intensity">Four components, the last one is the intensity.</param>
bool render_directional_light(
    const float* direction,
    const float* color_and_intensity);

/// <summary>
/// Renders a point light.
/// </summary>
/// <param name="position">The position of the light.</param>
/// <param name="radius">The radius of influence of the light.</param>
/// <param name="color_and_intensity">Four components, the last one is the intensity.</param>
bool render_point_light(
    const float* position,
    float radius,
    const float* color_and_intensity);

/// <summary>
/// Render a mesh with a texture.
/// </summary>
/// <param name="transform">The transform matrix.</param>
/// <param name="mesh">The mesh to render.</param>
/// <param name="texture">The texture to use.</param>
/// <param name="mul_color">Multiply color. Four components.</param>
/// <param name="add_color">Add color. Four components.</param>
bool render_mesh(
    const float* transform,
    mesh_handle mesh,
    texture_handle texture,
    const float* mul_color = nullptr,
    const float* add_color = nullptr);

/// <summary>
/// Render a debug line.
/// </summary>
/// <param name="from">The start of the line.</param>
/// <param name="to">The end of the line.</param>
/// <param name="color">The color of the line. Four components.</param>
/// <returns>True if the line can be rendered.</returns>
bool render_debug_line(
    const float* from,
    const float* to,
    const float* color);

/// <summary>
/// Render debug text.
/// </summary>
/// <param name="text">The text to render.</param>
/// <param name="position">The position of the text.</param>
/// <param name="color">The color of the text. Four components.</param>
/// <param name="size">The size of the text.</param>
/// <returns>True if the text can be rendered.</returns>
bool render_debug_text(
    const std::string& text,
    const float* position,
    const float* color,
    float size);

/// <summary>
/// Render the scene.
/// </summary>
/// <param name="view">The view matrix.</param>
/// <param name="projection">The projection matrix.</param>
void render(const float* view, const float* projection);

/// <summary>
/// Device specific functions. Used when the engine does not provide the functionality.
/// </summary>
namespace device
{
    /// <summary>
    /// Initialize the device
    /// Will create a window with the given width, height and title.
    /// Creates backbuffers on all platforms. Ready for rendering.
    /// </summary>
    bool initialize(const device_configuration& config);

    /// <summary>
    /// Shutdown the device. Destroys the window and backbuffers.
    /// </summary>
    void shutdown();

    /// <summary>
    /// Updates the device. Polls the window for events and swaps the backbuffer.
    /// </summary>
    void update();

    /// <summary>
    /// Returns true if the window is open.
    /// </summary>
    bool should_close();

    /// <summary>
    /// Returns a platform-dependent window pointer. It's up to the user to cast it to the right type.
    /// </summary>
    void* get_window();
}

/// <summary>
/// A handful of tools to load meshes and textures.
/// </summary>
namespace tools
{
    /// <summary>
    /// Loads an obj mesh from a string. Only supports triangles.
    /// </summary>
    mesh_handle load_obj_mesh(
        const std::string& obj_file_contents,
        const std::string& object_name = "");

    /// <summary>
    /// Loads a png texture from a vector of bytes.
    /// </summary>
    texture_handle load_png_texture(const std::vector<char>& png_file_contents);
}

}