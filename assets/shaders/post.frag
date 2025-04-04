#version 460 core
#extension GL_GOOGLE_include_directive : require

#include "locations.glsl"
#include "uniforms.glsl"

layout(location = 0) uniform sampler2D s_texture;

in  vec2 tex_coords;
out vec4 frag_color;

void main()
{
	vec3 scene_color = texture(s_texture, tex_coords).rgb;

	vec2 uv = gl_FragCoord.xy / bee_resolution;   
    uv *=  1.0 - uv.yx;   //vec2(1.0)- uv.yx; -> 1.-u.yx; Thanks FabriceNeyret !    
    float vig = uv.x*uv.y * 20.0; // multiply with sth for intensity    
    vig = pow(vig, 0.15); // change pow for modifying the extend of the  vignette
    scene_color = mix(scene_color, vec3(0.0), 1.0 - vig);
	frag_color = vec4(scene_color, 1.0);
}
