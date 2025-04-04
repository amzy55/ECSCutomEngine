#include <sstream>
#include <map>
#include "xsr.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace xsr;
using namespace std;
using namespace glm;

namespace
{
    unsigned cantor(unsigned a, unsigned b)
    {
        return (a + b + 1) * (a + b) / 2 + b;
    }

    unsigned tri_hash(unsigned a, unsigned b, unsigned int c)
    {
        return cantor(a, cantor(b, c));
    }
}

mesh_handle tools::load_obj_mesh(const std::string& data, const std::string& object_name)
{
    if (data.empty())
        return mesh_handle();

    std::stringstream ss(data);

    char input[512];
    char line[512];

    float x, y, z, u, v;
    unsigned k, l, m;

    vector<vec3>        positions;
    vector<vec2>        texcoords;
    vector<vec3>        normals;
    vector<vec3>        colors;
    vector<unsigned>    indices;

    vector<vec3>        final_positions;
    vector<vec2>        final_texcoords;
    vector<vec3>        final_normals;
    vector<vec3>        final_colors;
    vector<unsigned>    final_indices;

    unsigned idx = 0;
    map<unsigned, unsigned> index_for_vertex;

    bool search = !object_name.empty();
    bool found = !search;

    int first_position = 0;
    int first_texcoord = 0;
    int first_normal = 0;

    while (ss >> input)
    {
        if (search && input[0] == 'o')
        {
            if (!found)
            {
                string name;
                ss >> name;
                found = (name == object_name);
            }
            else
            {
				break;
			}
        }   
        
        if (input[0] == '#')
        {
            // Comment, skip line
            ss.getline(line, 255, '\n');
        }
        else if (strcmp(input, "v") == 0)
        {
            // Position
            ss >> x >> y >> z;
            if(found)
                positions.push_back(vec3(x, y, z));				
			else
                first_position++;
                
        }
        else if (strcmp(input, "vt") == 0)
        {
            // Texture coord
            ss >> u >> v;
            if(found)
                texcoords.push_back(vec2(u, v));				
			else
                first_texcoord++;
                
        }
        else if (strcmp(input, "vn") == 0)
        {
            // Vertex normal
            ss >> x >> y >> z;
            if(found)
                normals.push_back(vec3(x, y, z));                
            else
                first_normal++;
                
        }
        else if (strcmp(input, "vc") == 0)
        {
            // Vertex color
            ss >> x >> y >> z;
            colors.push_back(vec3(x, y, z));
        }               
        else if (strcmp(input, "f") == 0)
        {
            if (!found)
                continue;

            char slash;
            // Three vertices on a triangle
            for (int i = 0; i < 3; ++i)
            {
                // Read indices
                ss >> k >> slash >> l >> slash >> m;
                k -= 1;
                l -= 1;
                m -= 1;

                final_positions.push_back(positions[k - first_position]);
                final_texcoords.push_back(texcoords[l - first_texcoord]);
                final_normals.push_back(normals[m - first_normal]);
                final_colors.push_back(vec3(1,1,1)); // TODO: add support for colors
                final_indices.push_back(idx);
                idx++;


                /*
                // Read indices
                ss >> k >> slash >> l >> slash >> m;
                k -= 1;
                l -= 1;
                m -= 1;

                // Generate unique hash
                auto h = tri_hash(k, l, m);

                // Check if this a new vertex
                auto itr = index_for_vertex.find(h);
                // indexForVertex.end();//
                if ((false) && itr != index_for_vertex.end())
                {
                    // Vertex in set
                    indicesVec.push_back(itr->second);
                }
                else
                {
                    // New vertex
                    VertexPositionNormalTexture vertex;
                    vertex.Position = offsetVec[k];
                    vertex.Normal = normalVec[m];
                    vertex.Texture = textureVec[l];

                    // Add it to the vector
                    vertexVec.push_back(vertex);
                    // Book-keep index by vertex hash
                    indexForVertex[hash] = idx;
                    // Add index to indices vector
                    indicesVec.push_back(idx);
                    idx++;
                }
                */
            }
        }
    }

	return create_mesh(
        final_indices.data(),
        (unsigned)final_indices.size(),
        value_ptr(final_positions[0]),
        value_ptr(final_normals[0]),
        value_ptr(final_texcoords[0]),
        value_ptr(final_colors[0]),
        (unsigned)final_positions.size());
}

texture_handle tools::load_png_texture(const std::vector<char>& data)
{
	int width, height, channels;
    //stbi_set_flip_vertically_on_load(true);
	auto pixels = stbi_load_from_memory(
		reinterpret_cast<const unsigned char*>(data.data()),
		(int)data.size(),
		&width, &height, &channels, 4);
	if (!pixels)
		return texture_handle();
	auto handle = create_texture(width, height, pixels);
	stbi_image_free(pixels);
	return handle;
}



