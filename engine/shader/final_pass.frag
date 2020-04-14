R"(
#version 430 core

in vec2 vert_uv;

out vec4 frag_color;

layout (location = 1) uniform sampler2D final_texture;
layout (location = 2) uniform vec4 filter_color;

void main()
{
	frag_color = texture(final_texture, vert_uv) * filter_color;
}
)"