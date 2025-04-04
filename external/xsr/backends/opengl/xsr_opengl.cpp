#include <cassert>
#include <fstream>
#include <sstream>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#if defined(_WIN32)
#ifdef APIENTRY
#undef APIENTRY
#endif
#include <windows.h>
#endif

#include "xsr.hpp"

#define STRINGIFY(x) #x

using namespace xsr;
using namespace glm;

// #define XSR_USE_HARD_CODED_SHADERS

namespace xsr::internal
{
	GLFWwindow* window = nullptr;
	int	width;
	int	height;
	render_configuration rconfig;
	device_configuration dconfig;
	
	bool compile_standard_shader();
	bool compile_shader(GLuint* shader, GLenum type, const GLchar* source);
	bool link_program(GLuint program);
	void glfw_error_callback(int error, const char* description);	
	void init_debug_messages();
	void init_debug_variables();

#if defined(_DEBUG)
	static void debug_callback_func(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const GLvoid* userParam);

#if defined(_WIN32)
	void debug_callback_func_amd(
		GLuint id,
		GLenum category,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		void* userParam);	
#endif
#endif

	struct Mesh
	{		
		unsigned int				vao = 0;
		unsigned int				ebo = 0;
		std::array<unsigned int, 4>	vbos;
		uint32_t					count = 0;
	};

	struct Texture
	{
		unsigned int id = 0;
		int width = 0;
		int height = 0;
		int channels = 0;
	};

	struct DirectionalLight
	{
		vec3 direction;
		vec3 color;
	};

	struct PointLight
	{
		vec3 position;
		float range;
		vec3 color;
	};

	struct Entry
	{
		mat4 transform;
		mesh_handle mesh;
		texture_handle texture;
		vec4 mul_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vec4 add_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	};

	// All the meshes
	std::vector<Mesh> meshes;

	// All the textures
	std::vector<Texture> textures;

	unsigned int standard_program = 0;
	// unsigned int debug_program = 0;
	
	// All the active objects
	std::vector<DirectionalLight> dir_lights;
	std::vector<PointLight> point_lights;
	std::vector<Entry> entries;

#if defined(_DEBUG)
	static int const				m_maxLines = 16380;
	int								m_linesCount = 0;
	struct VertexPosition3DColor { glm::vec3 Position; glm::vec4 Color; };
	VertexPosition3DColor* m_vertexArray = nullptr;
	unsigned int					debug_program = 0;
	unsigned int					m_linesVAO = 0;
	unsigned int					m_linesVBO = 0;
#endif
}

using namespace xsr::internal;

bool xsr::initialize(const render_configuration& config)
{
	internal::rconfig = config;

	// TODO: Use the config
	// TODO: Handle errors and return false if something fails

	internal::init_debug_messages();
	internal::compile_standard_shader();
	internal::init_debug_variables();

	return true;
}

void xsr::shutdown()
{
	// Delete all the meshes
	for (auto& mesh : internal::meshes)
	{
		glDeleteVertexArrays(1, &mesh.vao);
		glDeleteBuffers(1, &mesh.ebo);
		glDeleteBuffers((GLsizei)mesh.vbos.size(), mesh.vbos.data());
	}

	// Delete all the textures
	for (auto& texture : internal::textures)
		glDeleteTextures(1, &texture.id);

	// Delete the standard shader
	glDeleteProgram(internal::standard_program);
}

mesh_handle xsr::create_mesh(
	const unsigned int* indices,
	unsigned int index_count,
	const float* positions,
	const float* normals,
	const float* texture_coordinates,
	const float* colors,
	unsigned int vertex_count)
{
	// Create an OpenGL mesh
	internal::Mesh mesh;
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	// Create the index buffer
	glGenBuffers(1, &mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(int), indices, GL_STATIC_DRAW);
	mesh.count = index_count;

	// Create the vertex buffers	
	glGenBuffers((GLsizei)mesh.vbos.size(), mesh.vbos.data());
	//glGenBuffers(1, mesh.vbos.data());

	// Create the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(float), positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Create the normal buffer
	if (normals)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(float), normals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	// Create the texture coordinate buffer
	if (texture_coordinates)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[2]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * 2 * sizeof(float), texture_coordinates, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	// Create the color buffer
	if (colors)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[3]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(float), colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);		
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	
	// Unbind the vertex array
	glBindVertexArray(0);
	
	// Store the mesh
	internal::meshes.push_back(mesh);
	return mesh_handle{ (int)internal::meshes.size() };
}

texture_handle xsr::create_texture(int width, int height, const void* pixel_rgba_bytes)
{
	// Create an OpenGL texture
	internal::Texture texture;
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_rgba_bytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Store the texture
	internal::textures.push_back(texture);
	return texture_handle{ (int)internal::textures.size() };
}

bool xsr::render_directional_light(const float* direction, const float* color_and_intensity)
{
	if(internal::dir_lights.size() >= 4)
		return false;

	// Add to the list of active directional lights	
	internal::dir_lights.push_back({
		{direction[0], direction[1], direction[2]},
		{color_and_intensity[0],color_and_intensity[1], color_and_intensity[2]}
	});

	return true;
}

bool xsr::render_point_light(const float* position, float radius, const float* color_and_intensity)
{
	if(internal::point_lights.size() >= 4)
		return false;

	// Add to the list of active point lights
	internal::point_lights.push_back({
		{ position[0], position[1], position[2]},
		radius,
		{color_and_intensity[0],color_and_intensity[1], color_and_intensity[2]}
	});

	return true;
}

bool xsr::render_mesh(
	const float* transform,
	mesh_handle mesh,
	texture_handle texture,
	const float* mul_color,
	const float* add_color)
{
	// Check that the mesh handle is valid
	if( mesh.id <= 0 || mesh.id > (int)meshes.size() )
		return false;

	// Check that the texture handle is valid
	if( texture.id <= 0 || texture.id > (int)textures.size() )
		return false; 

	// Add to the list of meshes to render
	Entry instance;
	instance.transform = glm::make_mat4(transform);
	instance.mesh = mesh;
	instance.texture = texture;
	if(mul_color)
		instance.mul_color = glm::make_vec4(mul_color);
	if(add_color)
		instance.add_color = glm::make_vec4(add_color);
	entries.push_back(instance);
	return true;
}

#if defined(_DEBUG)
bool xsr::render_debug_line(const float* from, const float* to, const float* color)
{
	if (m_linesCount < m_maxLines)
	{
		m_vertexArray[m_linesCount * 2].Position = make_vec3(from);
		m_vertexArray[m_linesCount * 2 + 1].Position = make_vec3(to);
		m_vertexArray[m_linesCount * 2].Color = make_vec4(color);
		m_vertexArray[m_linesCount * 2 + 1].Color = make_vec4(color);
		++m_linesCount;
		return true;
	}
	return false;
}

bool xsr::render_debug_text(
	const std::string& text,
	const float* position,
	const float* color,
	float size)
{
	return false;
}
#else

bool xsr::render_debug_line(const float* from, const float* to, const float* color)
{
	return true;
}

bool xsr::render_debug_text(
	const std::string& text,
	const float* position,
	const float* color,
	float size)
{
	return true;
}

#endif

void xsr::render(const float* view, const float* projection)
{
	// Clear the screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// Enable depth testing and set the depth function
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);

	// Activate the standard program
	const auto program = internal::standard_program;
	glUseProgram(program);

	// Send the view and projection matrices to the standard program
	glUniformMatrix4fv(glGetUniformLocation(program, "u_view"), 1, GL_FALSE, view);
	glUniformMatrix4fv(glGetUniformLocation(program, "u_projection"), 1, GL_FALSE, projection);
	auto texture_location = glGetUniformLocation(program, "u_texture");

	// Send the directional lights to the standard program
	glUniform1i(glGetUniformLocation(internal::standard_program, "u_num_directional_lights"), (int)internal::dir_lights.size());	

	for (int i = 0; i < internal::dir_lights.size(); ++i)
	{
		std::string name = "u_directional_lights[" + std::to_string(i) + "].direction";
		glUniform3fv(glGetUniformLocation(internal::standard_program, name.c_str()), 1, &internal::dir_lights[i].direction.x);
		name = "u_directional_lights[" + std::to_string(i) + "].color";
		glUniform3fv(glGetUniformLocation(internal::standard_program, name.c_str()), 1, &internal::dir_lights[i].color.x);
	}

	// Send the point lights to the standard program	
	glUniform1i(glGetUniformLocation(internal::standard_program, "u_num_point_lights"), (int)internal::point_lights.size());

	for (int i = 0; i < internal::point_lights.size(); ++i)
	{
		std::string name = "u_point_lights[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(internal::standard_program, (name + ".position").c_str()),
			1, &internal::point_lights[i].position.x);
		glUniform1f(glGetUniformLocation(internal::standard_program, (name + ".radius").c_str()),
			internal::point_lights[i].range);
		glUniform3fv(glGetUniformLocation(internal::standard_program, (name + ".color").c_str()),
			1, &internal::point_lights[i].color.x);
	}

	// Render each entry
	for (const internal::Entry& entry : internal::entries)
	{
		const auto& mesh = internal::meshes[entry.mesh.id - 1];
		const auto& texture = internal::textures[entry.texture.id - 1];
		const auto& transform = entry.transform;

		// Bind the vertex array
		glBindVertexArray(mesh.vao);

		// Bind the texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		// texture_location
		glUniform1i(texture_location, 0);

		// Send the model matrix to the standard program
		glUniformMatrix4fv(glGetUniformLocation(program, "u_model"), 1, GL_FALSE, value_ptr(entry.transform));

		// Send the multiply and add colors to the standard program
		glUniform4fv(glGetUniformLocation(program, "u_mul_color"), 1, value_ptr(entry.mul_color));
		glUniform4fv(glGetUniformLocation(program, "u_add_color"), 1, value_ptr(entry.add_color));

		// Draw the mesh
		glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_INT, 0);
	}

	// Clear the list of entries
	internal::entries.clear();

	// Clear the list of directional lights
	internal::dir_lights.clear();

	// Clear the list of point lights
	internal::point_lights.clear();

#if defined(_DEBUG)
	// Render debug lines
	glm::mat4 vp = make_mat4(projection) * make_mat4(view);
	glUseProgram(debug_program);
	glUniformMatrix4fv(1, 1, false, value_ptr(vp));
	glBindVertexArray(m_linesVAO);

	if (m_linesCount > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_linesVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPosition3DColor) * (m_maxLines * 2), &m_vertexArray[0], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINES, 0, m_linesCount * 2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	m_linesCount = 0;
#endif
}

// Create a  GLFW window
bool xsr::device::initialize(const device_configuration& config)
{
	width = config.width;
	height = config.height;
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

	window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return false;
	}	
	return true;
}

void xsr::device::shutdown()
{
	glfwTerminate();
}

void xsr::device::update()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

bool xsr::device::should_close()
{
	return glfwWindowShouldClose(window);
}

void* xsr::device::get_window()
{
	return window;
}

bool xsr::internal::compile_standard_shader()
{
#ifdef XSR_USE_HARD_CODED_SHADERS
	const char* vs_str = R"(
		#version 460 core
		layout(location = 0) in vec3 a_position;
		layout(location = 1) in vec3 a_normal;
		layout(location = 2) in vec2 a_texture_coordinate;
		layout(location = 3) in vec3 a_color;
		out vec3 v_normal;
		out vec2 v_texture_coordinate;
		out vec4 v_color;
		uniform mat4 u_model;
		uniform mat4 u_view;
		uniform mat4 u_projection;

		void main()
		{
			v_normal = a_normal;
			v_texture_coordinate = a_texture_coordinate;
			v_color = vec4(a_color, 1.0);
			gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
		}
	)";


	const char* fs_str = R"(
		#version 460 core
		in vec4 v_color;				
		in vec2 v_texture_coordinate;
		in vec3 v_normal;
		out vec4 frag_color;
		uniform sampler2D u_texture;
		uniform int u_num_directional_lights;
		uniform int u_num_point_lights;

		const int MAX_DIR_LIGHTS = 4;
		const int MAX_POINT_LIGHTS = 4;

		struct directional_light
		{
			vec3 direction;
			vec3 color;
		};

		struct point_light
		{
			vec3 position;
			vec3 color;
			float radius;
		};

		uniform directional_light u_directional_lights[MAX_DIR_LIGHTS];
		uniform point_light u_point_lights[MAX_POINT_LIGHTS];

		void main()
		{
			// Get diffuse color from texture
			vec4 texture_color = texture(u_texture, v_texture_coordinate);
	
			// Initialize diffuse color
			vec4 diffuse_color = vec4(0.0, 0.0, 0.0, 1.0);

			// calculate diffuse light intensity for directional lights		 
			for (int i = 0; i < u_num_directional_lights; i++)
			{
				directional_light light = u_directional_lights[i];
				diffuse_color += max(dot(normalize(v_normal), normalize(-light.direction)), 0.0);
			}

			// Get diffuse color from texture
			diffuse_color *= texture_color;

			// Get diffuse color from vertex color	
			// diffuse_color *= v_color;

			// Calculate diffuse light intensity
			//float diffuse = max(dot(normalize(v_normal), normalize(u_directional_light_direction)), 0.0);

			// Set output color
			frag_color = v_color * diffuse_color;
		} 
	)";
#else
	// Load the vertex and fragment shaders from files
	// using C++ streams and store them in C strings
	// This is only used when developing the shaders
	// In the release, copy/paste the contents of the files
	// into the strings above.
	std::ifstream vs_file(internal::rconfig.shader_path + "backends/opengl/standard.vert");
	std::ifstream fs_file(internal::rconfig.shader_path + "backends/opengl/standard.frag");
	std::stringstream vs_stream, fs_stream;
	vs_stream << vs_file.rdbuf();
	fs_stream << fs_file.rdbuf();
	std::string vs_str_s = vs_stream.str();
	std::string fs_str_s = fs_stream.str();
	const char* vs_str = vs_str_s.c_str();
	const char* fs_str = fs_str_s.c_str();
#endif

	GLuint vert_shader = 0;
	GLuint frag_shader = 0;

	standard_program = glCreateProgram();

	GLboolean res = compile_shader(&vert_shader, GL_VERTEX_SHADER, vs_str);
	if (!res)
		return false;
	
	res = compile_shader(&frag_shader, GL_FRAGMENT_SHADER, fs_str);
	if (!res)
	{
		printf("Renderer failed to compile sprite fragment shader");
		return false;
	}

	glAttachShader(standard_program, vert_shader);
	glAttachShader(standard_program, frag_shader);

	if (!link_program(standard_program))
	{
		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);
		glDeleteProgram(standard_program);
		printf("Renderer failed to link sprite shader program");
		return false;
	 }

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	return true;
}

bool xsr::internal::compile_shader(GLuint* shader, GLenum type, const GLchar* source)
{
	GLint status;

	if (!source)
	{
		printf("Failed to compile empty shader");
		return false;
	}

	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &source, nullptr);

	glCompileShader(*shader);

#if defined(_DEBUG)
	GLint log_length = 0;
	glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length > 1)
	{
		GLchar* log = static_cast<GLchar*>(malloc(log_length));
		glGetShaderInfoLog(*shader, log_length, &log_length, log);
		if (log)
			printf("Program compile log: %s", log);
		free(log);
	}
#endif

	glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
	if (status == 0)
	{
		glDeleteShader(*shader);
		return false;
	}

	return true;
}

bool xsr::internal::link_program(GLuint program)
{
	GLint status;
	glLinkProgram(program);

#if defined(_DEBUG)
	GLint logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1)
	{
		GLchar* log = static_cast<GLchar*>(malloc(logLength));
		glGetProgramInfoLog(program, logLength, &logLength, log);
		if (log)
			printf("Program link log: %s", log);
		free(log);
	}
#endif

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	return status != 0;
}

void xsr::internal::glfw_error_callback(int error, const char* description)
{

}

#if defined(_DEBUG)

#if defined(_WIN32)
void xsr::internal::init_debug_messages()
{
	// Query the OpenGL function to register your callback function.
	PFNGLDEBUGMESSAGECALLBACKPROC	 _glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
	PFNGLDEBUGMESSAGECALLBACKARBPROC _glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)wglGetProcAddress("glDebugMessageCallbackARB");
	PFNGLDEBUGMESSAGECALLBACKAMDPROC _glDebugMessageCallbackAMD = (PFNGLDEBUGMESSAGECALLBACKAMDPROC)wglGetProcAddress("glDebugMessageCallbackAMD");

	glDebugMessageCallback(xsr::internal::debug_callback_func, nullptr);

	// Register your callback function.
	if (_glDebugMessageCallback != nullptr)
	{
		_glDebugMessageCallback(xsr::internal::debug_callback_func, nullptr);
	}
	else if (_glDebugMessageCallbackARB != nullptr)
	{
		_glDebugMessageCallbackARB(xsr::internal::debug_callback_func, nullptr);
	}

	// Additional AMD support
	if (_glDebugMessageCallbackAMD != nullptr)
	{
		_glDebugMessageCallbackAMD(debug_callback_func_amd, nullptr);
	}

	// Enable synchronous callback. This ensures that your callback function is called
	// right after an error has occurred. This capability is not defined in the AMD
	// version.
	if ((_glDebugMessageCallback != nullptr) || (_glDebugMessageCallbackARB != nullptr))
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	glDebugMessageControl(
		GL_DONT_CARE,
		GL_DONT_CARE,
		GL_DEBUG_SEVERITY_LOW,
		0,
		nullptr,
		GL_FALSE);

	glDebugMessageControl(
		GL_DONT_CARE,
		GL_DONT_CARE,
		GL_DEBUG_SEVERITY_NOTIFICATION,
		0,
		nullptr,
		GL_FALSE);
}

void xsr::internal::init_debug_variables()
{
	m_vertexArray = new VertexPosition3DColor[m_maxLines * 2];

	const auto* const vsSource =
		"#version 460 core												\n\
		layout (location = 1) in vec3 a_position;						\n\
		layout (location = 2) in vec4 a_color;							\n\
		layout (location = 1) uniform mat4 u_worldviewproj;				\n\
		out vec4 v_color;												\n\
																		\n\
		void main()														\n\
		{																\n\
			v_color = a_color;											\n\
			gl_Position = u_worldviewproj * vec4(a_position, 1.0);		\n\
		}";

	const auto* const fsSource =
		"#version 460 core												\n\
		in vec4 v_color;												\n\
		out vec4 frag_color;											\n\
																		\n\
		void main()														\n\
		{																\n\
			frag_color = v_color;										\n\
		}";

	GLuint vertShader = 0;
	GLuint fragShader = 0;
	GLboolean res = GL_FALSE;

	debug_program = glCreateProgram();

	res = compile_shader(&vertShader, GL_VERTEX_SHADER, vsSource);
	if (!res)
	{
		printf("DebugRenderer failed to compile vertex shader");
		return;
	}

	res = compile_shader(&fragShader, GL_FRAGMENT_SHADER, fsSource);
	if (!res)
	{
		printf("DebugRenderer failed to compile fragment shader");
		return;
	}

	glAttachShader(debug_program, vertShader);
	glAttachShader(debug_program, fragShader);

	if (!link_program(debug_program))
	{
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(debug_program);
		printf("DebugRenderer failed to link shader program");
		return;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glCreateVertexArrays(1, &m_linesVAO);
	glBindVertexArray(m_linesVAO);

	// Allocate VBO
	glGenBuffers(1, &m_linesVBO);

	// Array buffer contains the attribute data
	glBindBuffer(GL_ARRAY_BUFFER, m_linesVBO);

	// Allocate into VBO
	const auto size = sizeof(m_vertexArray);
	glBufferData(GL_ARRAY_BUFFER, size, &m_vertexArray[0], GL_STREAM_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition3DColor),
		reinterpret_cast<void*>(offsetof(VertexPosition3DColor, Position)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosition3DColor),
		reinterpret_cast<void*>(offsetof(VertexPosition3DColor, Color)));

	glBindVertexArray(0); // TODO: Only do this when validating OpenGL
}

#else
void xsr::internal::init_debug_messages()
{
	glDebugMessageCallback(debug_callback_func, NULL);
}
#endif

void xsr::internal::debug_callback_func_amd(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar* message, void* userParam)
{
	xsr::internal::debug_callback_func(
		GL_DEBUG_CATEGORY_API_ERROR_AMD,
		category,
		id,
		severity,
		length,
		message,
		userParam);
}


void xsr::internal::debug_callback_func(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam)
{
	// Skip some less useful info
	if (id == 131218)	// http://stackoverflow.com/questions/12004396/opengl-debug-context-performance-warning
		return;

	// UNUSED(length);
	// UNUSED(userParam);
	std::string source_string;
	std::string type_string;
	std::string severity_string;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
	case GL_DEBUG_SOURCE_API: {
		source_string = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION: {
		source_string = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		source_string = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		source_string = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		source_string = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER: {
		source_string = "Other";
		break;
	}
	default: {
		source_string = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		type_string = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		type_string = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		type_string = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		type_string = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		type_string = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		type_string = "Other";
		break;
	}
	default: {
		type_string = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severity_string = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severity_string = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severity_string = "Low";
		break;
	}
	default: {
		severity_string = "Unknown";
		return;
	}
	}
	
	printf("%s:%s(%s): %s\n", source_string.c_str(), type_string.c_str(), severity_string.c_str(), message);
}
#else
void xsr::internal::init_debug_messages() {}
void xsr::internal::init_debug_variables() {}
#endif
