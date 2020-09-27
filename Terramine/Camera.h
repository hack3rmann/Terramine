#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include "defines.cpp"
#include "Window.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
using namespace glm;


class Camera {
	void updateVectors();
public:
	vec3 frontCam;
	vec3 frontMove;
	vec3 up;
	vec3 right;
	mat4 rotation;

	float fov;
	vec3 position;
	Camera(vec3 position, float fov);

	void rotate(float x, float y, float z);
	mat4 getProjection() const;
	mat4 getView() const;
};

#endif // !CAMERA_H_