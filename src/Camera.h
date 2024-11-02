#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "types.hpp"

using namespace glm;

class Camera {
public:
    void updateVectors();
    vec3 frontCam;
    vec3 frontMove;
    vec3 up;
    vec3 right;
    mat4 rotation;

    float fov;
    vec3 position;
    Camera(vec3 position, float fov);

    void rotate(float x, float y, float z);
    mat4 getProjection(tmine::f32 aspect_ratio) const;
    mat4 getOrtho() const;
    mat4 getView() const;
};
