#include "Player.h"

#include <GLFW/glfw3.h>

#include "Voxels/Voxel.h"
#include "Window.h"
#include "events.hpp"

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
    if (io.just_pressed(Key::F)) {
        isSpeedUp = !isSpeedUp;
    }

    for (u32 i = 0; i < 9; i++) {
        auto key = Key{(u32) Key::Key1 + i};

        if (io.just_pressed(key)) {
            currentBlock = i + 1;
        }
    }
    if (io.just_pressed(Key::Key0)) {
        currentBlock = 10;
    }
    if (io.just_pressed(Key::Minus)) {
        currentBlock = 11;
    }

    float speedFactor = 2.0f;

    /* Movement */
    if (io.is_pressed(Key::W) &&
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
    if (io.is_pressed(Key::W)) {
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
    if (io.is_pressed(Key::S)) {
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
    if (io.is_pressed(Key::A)) {
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
    if (io.is_pressed(Key::D)) {
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
    if (io.is_pressed(Key::LeftShift)) {
        if (chunks->rayCast(cam->position, -cam->up, 2.6f) == nullptr)
            ;
    }
    if (io.is_pressed(Key::Space)) {
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

    if (io.is_pressed(Key::P)) {
        cam->position = vec3(2.0f, 70.0f, 2.0f);
        speed = vec3(0.0f);
    }

    cam->position += speed * dTime;
    speed.x = 0.0f;
    speed.z = 0.0f;

    camX -= io.get_mouse_delta().y / Window::height * 2.f;
    camY -= io.get_mouse_delta().x / Window::width * 2.f;
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

            if (io.just_clicked(MouseButton::Left)) {
                chunks->set((int) iend.x, (int) iend.y, (int) iend.z, 0);
            }

            if (io.just_clicked(MouseButton::Right)) {
                chunks->set(
                    (int) iend.x + (int) norm.x, (int) iend.y + (int) norm.y,
                    (int) iend.z + (int) norm.z, currentBlock
                );
            }
        }
    }
}
