
#include "renderer.h"
#include "sprite_cache.h"

#include "gl_err.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <optional>
#include <string>

namespace {

	struct Renderable {
		glm::mat4 transform = glm::mat4(1.0f);
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		u32 sprite_index = 0;
		u32 pad[3];
	};

	std::optional<u32> vao;
	std::optional<u32> vbo;
	std::optional<u32> view_ubo;
	std::optional<u32> proj_ubo;
	std::optional<u32> renderable_ssbo;
	std::vector<Renderable> staging_buffer;
	struct {
		std::optional<u32> framebuffer;
		std::optional<u32> color_attachment;
		std::optional<u32> shader;
	} world_draw;
	struct {
		std::optional<u32> framebuffer;
		std::optional<u32> color_attachment;
		std::optional<u32> shader;
	} screen_draw;
	struct {
		std::optional<u32> shader;
	} final_draw;

	i32 compileShader(std::string vert, std::string frag) {

		const char* vert_src_c_str = vert.c_str();
		const char* frag_src_c_str = frag.c_str();

		unsigned int vertex, fragment;
		int success;
		char infoLog[512];
		u32 program_id = 0;

		// vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vert_src_c_str, NULL);
		glCompileShader(vertex);
		// print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR: Vertex shader compilation failed, skipping shader creation." << infoLog << std::endl;
			return false;
		}
		else {
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &frag_src_c_str, NULL);
			glCompileShader(fragment);
			// print compile errors if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::cout << "ERROR: Fragment shader compilation failed, skipping shader creation." << infoLog << std::endl;
				glDeleteShader(vertex);
				return false;
			}
			else {
				// shader Program
				program_id = glCreateProgram();
				glAttachShader(program_id, vertex);
				glAttachShader(program_id, fragment);
				glLinkProgram(program_id);
				// print linking errors if any
				glGetProgramiv(program_id, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(program_id, 512, NULL, infoLog);
					std::cout << "ERROR: Shader program creation failed, skipping shader creation." << infoLog << std::endl;
					glDeleteShader(vertex);
					glDeleteShader(fragment);
					program_id = 0;
					return false;
				}

			}
		}

		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		if (success) {
			return program_id;
		}
		return -1;

	}

}

void Renderer::startup() {

	const Config& config = Config::get();

	u32 buffers[2];
	glCreateBuffers(2, buffers);
	vbo = buffers[0];
	//view_ubo = buffers[1];
	//proj_ubo = buffers[2];
	renderable_ssbo = buffers[1];
	u32 new_vao;
	glGenVertexArrays(1, &new_vao);
	vao = new_vao;

	// create the framebuffers
	u32 framebuffers[2];
	glGenFramebuffers(2, framebuffers);
	world_draw.framebuffer = framebuffers[0];
	screen_draw.framebuffer = framebuffers[1];

	u32 color_attachement;
	glBindFramebuffer(GL_FRAMEBUFFER, world_draw.framebuffer.value());
	glGenTextures(1, &color_attachement);
	glBindTexture(GL_TEXTURE_2D, color_attachement);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config.viewport_size.x, config.viewport_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachement, 0);
	world_draw.color_attachment = color_attachement;

	glBindFramebuffer(GL_FRAMEBUFFER, screen_draw.framebuffer.value());
	glGenTextures(1, &color_attachement);
	glBindTexture(GL_TEXTURE_2D, color_attachement);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config.viewport_size.x, config.viewport_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachement, 0);
	screen_draw.color_attachment = color_attachement;

	//glBindBufferBase(GL_UNIFORM_BUFFER, 0, proj_ubo.value());
	//glm::mat4 proj = glm::ortho(-((f32)config.viewport_size.x / 2), ((f32)config.viewport_size.x / 2), -((f32)config.viewport_size.y / 2), ((f32)config.viewport_size.y / 2), -64.0f, 64.0f);
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), &proj_ubo.value(), GL_STATIC_DRAW);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::string world_vert =
#include "shader/world_pass.vert"
		;
	std::string world_frag =
#include "shader/world_pass.frag" 
		;

	std::string screen_vert =
#include "shader/screen_pass.vert"
		;
	std::string screen_frag =
#include "shader/screen_pass.frag" 
		;

	std::string final_vert =
#include "shader/final_pass.vert"
		;
	std::string final_frag =
#include "shader/final_pass.frag" 
		;

	world_draw.shader = compileShader(world_vert, world_frag);
	screen_draw.shader = compileShader(screen_vert, screen_frag);
	final_draw.shader = compileShader(final_vert, final_frag);

	CheckGLError();

}

void Renderer::shutdown() {

	glDeleteProgram(world_draw.shader.value());
	glDeleteFramebuffers(1, &world_draw.framebuffer.value());
	glDeleteTextures(1, &world_draw.color_attachment.value());

	glDeleteFramebuffers(1, &screen_draw.framebuffer.value());
	glDeleteTextures(1, &screen_draw.color_attachment.value());

	//glDeleteBuffers(1, &view_ubo.value());
	//glDeleteBuffers(1, &proj_ubo.value());
	glDeleteBuffers(1, &renderable_ssbo.value());

}

void Renderer::draw(Frame& frame) {

	const Config& config = Config::get();

	glm::mat4 proj = glm::ortho(-((f32)config.viewport_size.x / 2), ((f32)config.viewport_size.x / 2), -((f32)config.viewport_size.y / 2), ((f32)config.viewport_size.y / 2), -64.0f, 64.0f);
	//glm::mat4 proj = glm::ortho(-((f32)1920 / 2), ((f32)1920 / 2), -((f32)1080 / 2), ((f32)1080 / 2), -64.0f, 64.0f);
	glm::mat4 view = glm::mat4(1.0);
	view = glm::translate(view, glm::vec3(frame.camera.pos * (f32)config.unit_pixels, 0.0f));
	view = glm::scale(view, glm::vec3(frame.camera.zoom, frame.camera.zoom, frame.camera.zoom));

	// World draw

	staging_buffer.resize(0);
	for (auto& command : frame.world_render_commands) {
		Renderable new_renderable;
		new_renderable.sprite_index = command.sprite_handle;
		new_renderable.transform = glm::translate(new_renderable.transform, glm::vec3(command.pos.x * config.unit_pixels, command.pos.y * config.unit_pixels, 0.0f));
		new_renderable.transform = glm::rotate(new_renderable.transform, glm::radians(command.rot), glm::vec3(0.0f, 0.0f, 1.0f));
		new_renderable.transform = glm::scale(new_renderable.transform, glm::vec3(command.scale, 0.0f));
		new_renderable.color = command.color;
		staging_buffer.push_back(new_renderable);
	}
	frame.world_render_commands.resize(0);
	
	glViewport(0, 0, config.viewport_size.x, config.viewport_size.y);

	glBindFramebuffer(GL_FRAMEBUFFER, world_draw.framebuffer.value()); CheckGLError();
	glClearColor(0.25, 0.90, 1.0, 1.0); CheckGLError();
	glClear(GL_COLOR_BUFFER_BIT); CheckGLError();
	glUseProgram(world_draw.shader.value()); CheckGLError();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(proj)); CheckGLError();
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view)); CheckGLError();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SpriteCache::getSSBO().value()); CheckGLError();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, renderable_ssbo.value()); CheckGLError();
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Renderable) * staging_buffer.size(), staging_buffer.data(), GL_DYNAMIC_DRAW); CheckGLError();
	glBindVertexArray(vao.value()); CheckGLError();
	glActiveTexture(GL_TEXTURE0); CheckGLError();
	glBindTexture(GL_TEXTURE_2D, SpriteCache::getTexture().value()); CheckGLError();
	glDrawArrays(GL_TRIANGLES, 0, staging_buffer.size() * 6); CheckGLError();

	// Screen draw

	staging_buffer.resize(0);
	for (auto& command : frame.screen_render_commands) {
		Renderable new_renderable;
		new_renderable.sprite_index = command.sprite_handle;
		new_renderable.transform = glm::translate(new_renderable.transform, glm::vec3(command.pos.x * config.unit_pixels, command.pos.y * config.unit_pixels, 0.0f));
		new_renderable.transform = glm::rotate(new_renderable.transform, glm::radians(command.rot), glm::vec3(0.0f, 0.0f, 1.0f));
		new_renderable.transform = glm::scale(new_renderable.transform, glm::vec3(command.scale, 0.0f));
		new_renderable.color = command.color;
		staging_buffer.push_back(new_renderable);
	}
	frame.screen_render_commands.resize(0);

	glBindFramebuffer(GL_FRAMEBUFFER, screen_draw.framebuffer.value()); CheckGLError();
	glClearColor(0.0, 0.0, 0.0, 0.0); CheckGLError();
	glClear(GL_COLOR_BUFFER_BIT); CheckGLError();
	glUseProgram(screen_draw.shader.value()); CheckGLError();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(proj)); CheckGLError();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SpriteCache::getSSBO().value()); CheckGLError();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, renderable_ssbo.value()); CheckGLError();
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Renderable) * staging_buffer.size(), staging_buffer.data(), GL_DYNAMIC_DRAW); CheckGLError();
	glBindVertexArray(vao.value()); CheckGLError();
	glActiveTexture(GL_TEXTURE0); CheckGLError();
	glBindTexture(GL_TEXTURE_2D, SpriteCache::getTexture().value()); CheckGLError();
	glDrawArrays(GL_TRIANGLES, 0, staging_buffer.size() * 6); CheckGLError();

	// Final draw

	glViewport(0, 0, frame.window_size.x, frame.window_size.y);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); CheckGLError();
	glUseProgram(final_draw.shader.value()); CheckGLError();
	glUniform4f(2, frame.world_color.x, frame.world_color.y, frame.world_color.z, frame.world_color.w);
	glClearColor(0.0, 0.0, 0.0, 1.0); CheckGLError();
	glClear(GL_COLOR_BUFFER_BIT); CheckGLError();
	glBindTexture(GL_TEXTURE_2D, world_draw.color_attachment.value()); CheckGLError();
	glDrawArrays(GL_TRIANGLES, 0, 6); CheckGLError();
	glUniform4f(2, 1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, screen_draw.color_attachment.value()); CheckGLError();
	glDrawArrays(GL_TRIANGLES, 0, 6); CheckGLError();
	
	// Screen draw
	glBindVertexArray(0);

}