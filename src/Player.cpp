#include "Player.h"

#include <GLFW/glfw3.h>

#include "Voxels/Voxel.h"
#include "EventHandler.h"
#include "Window.h"

using namespace tmine;

Player::Player() {
    lineBatch = new LineBox();
    camX = camY = 0.0f;
    currentBlock = 1;
}

Player::Player(float lastTime, float gravity, vec3 speed)
    : lastTime(lastTime)
    , gravity(gravity)
    , speed(speed) {
    cam = new Camera(vec3(32.0f, 70.0f, 32.0f), radians(60.0f));
    camX = camY = 0.0f;
    currentBlock = 1;
}

void Player::updateTime() {
    currTime = glfwGetTime();
    dTime = currTime - lastTime;
    lastTime = currTime;
}

void Player::update(Chunks* chunks, LineBox* lineBatch) {
    updateTime();
    if (Events::justPressed(GLFW_KEY_F)) {
        isSpeedUp = !isSpeedUp;
    }

    for (unsigned int i = 0; i < 9; i++) {
        if (Events::justPressed(GLFW_KEY_1 + i)) {
            currentBlock = i + 1;
        }
    }
    if (Events::justPressed(GLFW_KEY_0)) {
        currentBlock = 10;
    }
    if (Events::justPressed(GLFW_KEY_MINUS)) {
        currentBlock = 11;
    }

    float speedFactor = 2.0f;

    /* Movement */
    if (Events::isPressed(GLFW_KEY_W) &&
        chunks->rayCast(
            vec3(cam->position.x, cam->position.y - 2.0f, cam->position.z),
            cam->frontMove, 0.3f
        ) != nullptr &&
        chunks->rayCast(
            vec3(cam->position.x, cam->position.y - 1.0f, cam->position.z),
            cam->frontMove, 0.3f
        ) == nullptr &&
        chunks->rayCast(
            vec3(cam->position.x, cam->position.y + 1.0f, cam->position.z),
            cam->frontMove, 0.3f
        ) == nullptr &&
        chunks->rayCast(cam->position, cam->frontMove, 0.3f) == nullptr)
    {
        speed.y = 8.5f;
    }
    if (Events::isPressed(GLFW_KEY_W)) {
        vec3 norm;
        if (chunks->rayCast(cam->position, cam->frontMove, 0.3f, norm) ==
                nullptr &&
            chunks->rayCast(
                vec3(cam->position.x, cam->position.y - 1.0f, cam->position.z),
                cam->frontMove, 0.3f, norm
            ) == nullptr &&
            chunks->rayCast(
                vec3(cam->position.x, cam->position.y - 2.0f, cam->position.z),
                cam->frontMove, 0.3f, norm
            ) == nullptr)
        {
            speed += cam->frontMove / dTime / 10.0f * speedFactor;
        } else {
            vec3 dir(norm.z, norm.x, -norm.y);
            float len = length(dir);
            float DdotC = dot(dir, cam->frontMove / dTime / 10.0f);
            if (len != 0.0f) {
                vec3 s = dir * DdotC / len;
                if (s != vec3(0.0f)) {
                    speed += s * speedFactor;
                } else {
                    dir = vec3(-norm.y, norm.z, norm.x);
                    speed += dir * DdotC / len * speedFactor;
                }
            }
        }
    }
    if (Events::isPressed(GLFW_KEY_S)) {
        vec3 norm;
        if (chunks->rayCast(cam->position, -cam->frontMove, 0.3f, norm) ==
                nullptr &&
            chunks->rayCast(
                vec3(cam->position.x, cam->position.y - 1.0f, cam->position.z),
                -cam->frontMove, 0.3f, norm
            ) == nullptr &&
            chunks->rayCast(
                vec3(cam->position.x, cam->position.y - 2.0f, cam->position.z),
                -cam->frontMove, 0.3f, norm
            ) == nullptr)
        {
            speed -= cam->frontMove / dTime / 10.0f * speedFactor;
        } else {
            vec3 dir(norm.z, norm.x, -norm.y);
            speed += dir * dot(dir, -cam->frontMove / dTime / 10.0f) /
                     length(dir) * speedFactor;
        }
    }
    if (Events::isPressed(GLFW_KEY_A)) {
        vec3 norm;
        if (chunks->rayCast(cam->position, -cam->right, 0.3f, norm) ==
                nullptr &&
            chunks->rayCast(
                vec3(cam->position.x, cam->position.y - 1.0f, cam->position.z),
                -cam->right, 0.3f, norm
            ) == nullptr &&
            chunks->rayCast(
                vec3(cam->position.x, cam->position.y - 2.0f, cam->position.z),
                -cam->right, 0.3f, norm
            ) == nullptr)
        {
            speed -= cam->right / dTime / 10.0f * speedFactor;
        } else {
            vec3 dir(norm.z, norm.x, -norm.y);
            speed += dir * dot(dir, -cam->right / dTime / 10.0f) / length(dir) *
                     speedFactor;
        }
    }
    if (Events::isPressed(GLFW_KEY_D)) {
        vec3 norm;
        if (chunks->rayCast(cam->position, cam->right, 0.3f, norm) == nullptr &&
            chunks->rayCast(
                vec3(cam->position.x, cam->position.y - 1.0f, cam->position.z),
                cam->right, 0.3f, norm
            ) == nullptr &&
            chunks->rayCast(
                vec3(cam->position.x, cam->position.y - 2.0f, cam->position.z),
                cam->right, 0.3f, norm
            ) == nullptr)
        {
            speed += cam->right / dTime / 10.0f * speedFactor;
        } else {
            vec3 dir(norm.z, norm.x, -norm.y);
            speed += dir * dot(dir, cam->right / dTime / 10.0f) / length(dir) *
                     speedFactor;
        }
    }
    if (Events::isPressed(GLFW_KEY_LEFT_SHIFT)) {
        if (chunks->rayCast(cam->position, -cam->up, 2.6f) == nullptr)
            ;
    }
    if (Events::isPressed(GLFW_KEY_SPACE)) {
        if (chunks->rayCast(cam->position, -cam->up, 2.6f) != nullptr) {
            speed.y = 20.0f;
        }
    }

    if (chunks->rayCast(
            cam->position, -cam->up, abs(speed.y * dTime * dTime) + 2.6f
        ) == nullptr)
    {
        speed.y += gravity * dTime;
    } else {
        if (speed.y < 0) {
            speed.y = 0.0f;
        }
    }

    if (chunks->rayCast(
            cam->position, cam->up, abs(speed.y * dTime * dTime) + 0.2f
        ) != nullptr &&
        speed.y > 0)
    {
        speed.y = -speed.y * 0.5;
    }

    if (Events::isPressed(GLFW_KEY_P)) {
        cam->position = vec3(2.0f, 70.0f, 2.0f);
        speed = vec3(0.0f);
    }

    cam->position += speed * dTime;
    speed.x = 0.0f;
    speed.z = 0.0f;

    camX -= Events::dy / Window::height * 2.f;
    camY -= Events::dx / Window::width * 2.f;
    cam->rotation = mat4(1.0f);
    if (camX > radians(89.9f)) {
        camX = radians(89.9f);
    }
    if (camX < radians(-89.9f)) {
        camX = radians(-89.9f);
    }
    cam->rotate(camX, camY, 0.0f);

    /* Interacting -> raycasting */ {
        vec3 end;
        vec3 norm;
        vec3 iend;
        Voxel* vox = chunks->rayCast(
            cam->position, cam->frontCam, 10.0f, end, norm, iend
        );
        if (vox != nullptr) {
            lineBatch->box(
                iend + 0.5f, glm::vec3(1.001f), glm::vec4(glm::vec3(60.0f / 255.0f), 0.5f));

            if (Events::justClicked(GLFW_MOUSE_BUTTON_1)) {
                chunks->set((int) iend.x, (int) iend.y, (int) iend.z, 0);
            }
            if (Events::justClicked(GLFW_MOUSE_BUTTON_2)) {
                chunks->set(
                    (int) iend.x + (int) norm.x, (int) iend.y + (int) norm.y,
                    (int) iend.z + (int) norm.z, currentBlock
                );
            }
        }
    }
}
