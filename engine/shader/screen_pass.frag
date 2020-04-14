R"(
#version 430 core

in vec4 vert_color;
in vec2 vert_uv;

out vec4 frag_color;

layout(location = 1) uniform sampler2D sprite_texture;

void main()
{
	frag_color = texture(sprite_texture, vert_uv) * vert_color;
}
)"