#version 450 core
layout(location = 0) in vec2 l_pos;
layout(location = 1) in vec2 l_texels;

out float io_opacity;
out vec2 io_texels;

uniform mat4 u_projection;
uniform float u_opacity;

void main()
{
	gl_Position = u_projection * vec4(l_pos, 0.0, 1.0);
	
	io_texels = l_texels;
	io_opacity = u_opacity;
}