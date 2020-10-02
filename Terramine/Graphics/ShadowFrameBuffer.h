#pragma once

#include <GL/glew.h>

class ShadowFB {
	friend class SceneHandler;

	GLuint fbo;
public:
	ShadowFB();
	void terminate();
};