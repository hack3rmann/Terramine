#pragma once

#include "GL/glew.h"
#include "Shader.h"
#include <string>

class FrameBuffer {
	friend class SceneHandler;
	std::string vName, fName;

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
	FrameBuffer(const std::string& vName, const std::string& fName);
	FrameBuffer();
	void terminate();
	void bind();
	void drawColor();
	void drawDepth();
	void drawBoth();
	void bindColorTex();
	void bindDepthTex();
	void reload();
	void reload(const std::string& vName, const std::string& fName);
	void refreshShader();
	int check();
	static void unbind();
};