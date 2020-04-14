R"(
#version 430 core

vec2 vertices[6] = {
	// first triangle
	vec2(1.0f,  1.0f),  // top right
	vec2(1.0f, -1.0f),  // bottom right
	vec2(-1.0f,  1.0f),  // top left 
	// second triangle
	vec2(1.0f, -1.0f),  // bottom right
	vec2(-1.0f, -1.0f),  // bottom left
	vec2(-1.0f,  1.0f)   // top left
};

// Flipped on y
vec2 uvs[6] = {
	// first triangle
	vec2(1.0f, 0.0f),  // top right
	vec2(1.0f, 1.0f),  // bottom right
	vec2(0.0f, 0.0f),  // top left 
	// second triangle
	vec2(1.0f, 1.0f),  // bottom right
	vec2(0.0f, 1.0f),  // bottom left
	vec2(0.0f, 0.0f)   // top left
};

struct Sprite {
	vec4 uv;
};

struct Renderable {
	mat4 transform;
	vec4 color;
	uint sprite_index;
};

layout (std430, binding=0) buffer sprite_buffer
{ 
	Sprite sprites[];
};

layout (std430, binding=1) buffer draw_buffer
{ 
	Renderable renderables[];
};

layout (location=0) uniform mat4 proj;
layout (location=1) uniform mat4 view;

out vec4 vert_color;
out vec2 vert_uv;

void main(void) {

	// Resove id's
	int instance_id = gl_VertexID / 6;
	int vertex_id = gl_VertexID % 6;
	Renderable renderable = renderables[instance_id];
	Sprite sprite = sprites[renderable.sprite_index];
	vec2 source_uv = uvs[vertex_id];

	// I know there is a much better way to do this but its working right now so not a priority
	mat4 sprite_transform;
	sprite_transform[0] = vec4(sprite.uv.z, 0.0, 0.0, 0.0);
	sprite_transform[1] = vec4(0.0, sprite.uv.w, 0.0, 0.0);
	sprite_transform[2] = vec4(0.0, 0.0, 0.0, 0.0);
	sprite_transform[3] = vec4(0.0, 0.0, 0.0, 1.0);

	//renderable.transform[0].x *= sprite.uv.z;
	//renderable.transform[1].y *= sprite.uv.w;

	// Nullify any z value as the layer setup already dictates draw order
	renderable.transform[3].z = 0.0;

	vert_uv = vec2((sprite.uv.x / float(512)) + (source_uv.x * (sprite.uv.z / float(512))), 1.0 - ((sprite.uv.y / float(512)) + (source_uv.y * (sprite.uv.w / float(512)))));
	vert_color = renderable.color;
    gl_Position = proj * view * renderable.transform * sprite_transform * vec4(vertices[vertex_id], 0.0f, 1.0f);
}
)"