#pragma once

#include "GL/glew.h"
#include "Shader.h"

class FrameBuffer {
	friend class SceneHandler;

	Shader* screenShader;
	static float screenQuad[24];
	GLuint vao;
	GLuint vbo;

	GLuint fbo;
	GLuint color_rbo;
	GLuint depth_rbo;
	GLuint colorAtt;
	GLuint depthAtt;
	int msaa;

	int width, height;
public:
	FrameBuffer();
	void terminate();
	void bind();
	void draw();
	void reload();
	void refreshShader();
	int check();
	static void unbind();
};