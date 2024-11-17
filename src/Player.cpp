#include "Player.h"
#include "events.hpp"

using namespace tmine;

Player::Player()
: cam{vec3(32.0f, 70.0f, 32.0f), radians(60.0f)} {
    camX = camY = 0.0f;
    currentBlock = 1;
}

Player::Player(float lastTime, float gravity, vec3 speed)
: lastTime(lastTime)
, gravity(gravity)
, cam{vec3(32.0f, 70.0f, 32.0f), radians(60.0f)}
, speed(speed) {
    camX = camY = 0.0f;
    currentBlock = 1;
}

void Player::updateTime() {
    currTime = glfwGetTime();
    dTime = currTime - lastTime;
    lastTime = currTime;
}

void Player::update(
    RefMut<BoxCollider> collider, RefMut<Terrain> terrain,
    RefMut<LineBox> lineBatch, glm::uvec2 window_size
) {
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
        terrain->get_array()
            .ray_cast(
                vec3(cam.get_pos().x, cam.get_pos().y - 2.0f, cam.get_pos().z),
                cam.get_move_direction(), 0.3f
            )
            .has_hit &&
        terrain->get_array()
            .ray_cast(
                vec3(cam.get_pos().x, cam.get_pos().y - 1.0f, cam.get_pos().z),
                cam.get_move_direction(), 0.3f
            )
            .has_hit &&
        terrain->get_array()
            .ray_cast(
                vec3(cam.get_pos().x, cam.get_pos().y + 1.0f, cam.get_pos().z),
                cam.get_move_direction(), 0.3f
            )
            .has_hit &&
        terrain->get_array()
            .ray_cast(cam.get_pos(), cam.get_move_direction(), 0.3f)
            .has_hit)
    {
        speed.y = 8.5f;
    }
    if (io.is_pressed(Key::W)) {
        auto res1 = terrain->get_array().ray_cast(
            cam.get_pos(), cam.get_move_direction(), 0.3f
        );
        auto res2 = terrain->get_array().ray_cast(
            vec3(cam.get_pos().x, cam.get_pos().y - 1.0f, cam.get_pos().z),
            cam.get_move_direction(), 0.3f
        );
        auto res3 = terrain->get_array().ray_cast(
            vec3(cam.get_pos().x, cam.get_pos().y - 2.0f, cam.get_pos().z),
            cam.get_move_direction(), 0.3f
        );

        auto norm = res3.normal;

        if (!res1.has_hit && !res2.has_hit && !res3.has_hit) {
            speed += cam.get_move_direction() / dTime / 10.0f * speedFactor;
        } else {
            vec3 dir(norm.z, norm.x, -norm.y);
            float len = length(dir);
            float DdotC = dot(dir, cam.get_move_direction() / dTime / 10.0f);
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
        auto res1 = terrain->get_array().ray_cast(
            cam.get_pos(), -cam.get_move_direction(), 0.3f
        );
        auto res2 = terrain->get_array().ray_cast(
            vec3(cam.get_pos().x, cam.get_pos().y - 1.0f, cam.get_pos().z),
            -cam.get_move_direction(), 0.3f
        );
        auto res3 = terrain->get_array().ray_cast(
            vec3(cam.get_pos().x, cam.get_pos().y - 2.0f, cam.get_pos().z),
            -cam.get_move_direction(), 0.3f
        );

        auto norm = res3.normal;

        if (!res1.has_hit && !res2.has_hit && !res3.has_hit) {
            speed -= cam.get_move_direction() / dTime / 10.0f * speedFactor;
        } else {
            vec3 dir(norm.z, norm.x, -norm.y);
            speed += dir * dot(dir, -cam.get_move_direction() / dTime / 10.0f) /
                     length(dir) * speedFactor;
        }
    }
    if (io.is_pressed(Key::A)) {
        auto res1 = terrain->get_array().ray_cast(
            cam.get_pos(), -cam.get_right_direction(), 0.3f
        );
        auto res2 = terrain->get_array().ray_cast(
            vec3(cam.get_pos().x, cam.get_pos().y - 1.0f, cam.get_pos().z),
            -cam.get_right_direction(), 0.3f
        );
        auto res3 = terrain->get_array().ray_cast(
            vec3(cam.get_pos().x, cam.get_pos().y - 2.0f, cam.get_pos().z),
            -cam.get_right_direction(), 0.3f
        );

        auto norm = res3.normal;

        if (!res1.has_hit && !res2.has_hit && !res3.has_hit) {
            speed -= cam.get_right_direction() / dTime / 10.0f * speedFactor;
        } else {
            vec3 dir(norm.z, norm.x, -norm.y);
            speed += dir *
                     dot(dir, -cam.get_right_direction() / dTime / 10.0f) /
                     length(dir) * speedFactor;
        }
    }
    if (io.is_pressed(Key::D)) {
        auto res1 = terrain->get_array().ray_cast(
            cam.get_pos(), cam.get_right_direction(), 0.3f
        );
        auto res2 = terrain->get_array().ray_cast(
            vec3(cam.get_pos().x, cam.get_pos().y - 1.0f, cam.get_pos().z),
            cam.get_right_direction(), 0.3f
        );
        auto res3 = terrain->get_array().ray_cast(
            vec3(cam.get_pos().x, cam.get_pos().y - 2.0f, cam.get_pos().z),
            cam.get_right_direction(), 0.3f
        );

        auto norm = res3.normal;

        if (!res1.has_hit && !res2.has_hit && !res3.has_hit) {
            speed += cam.get_right_direction() / dTime / 10.0f * speedFactor;
        } else {
            vec3 dir(norm.z, norm.x, -norm.y);
            speed += dir * dot(dir, cam.get_right_direction() / dTime / 10.0f) /
                     length(dir) * speedFactor;
        }
    }
    if (io.is_pressed(Key::Space)) {
        if (terrain->get_array()
                .ray_cast(cam.get_pos(), -cam.get_up_direction(), 2.6f)
                .has_hit)
        {
            speed.y = 20.0f;
        }
    }

#define GRAVITY 1

    if (!terrain->get_array()
             .ray_cast(
                 cam.get_pos(), -cam.get_up_direction(),
                 abs(speed.y * dTime * dTime) + 2.6f
             )
             .has_hit)
    {
#if GRAVITY
        speed.y += gravity * dTime;
#endif
    } else {
        if (speed.y < 0) {
            speed.y = 0.0f;
        }
    }

    if (terrain->get_array()
            .ray_cast(
                cam.get_pos(), cam.get_up_direction(),
                abs(speed.y * dTime * dTime) + 0.2f
            )
            .has_hit &&
        speed.y > 0)
    {
#if GRAVITY
        speed.y = -speed.y * 0.5;
#endif
    }

    if (io.is_pressed(Key::P)) {
        auto const sizes = glm::vec3{Chunk::SIZE * terrain->get_array().size()};

        cam.set_pos({0.5f * sizes.x, sizes.y, 0.5f * sizes.z});
        speed = vec3(0.0f);
    }

    cam.offset(speed * dTime);
    speed.x = 0.0f;
    speed.z = 0.0f;

    camX -= io.get_mouse_delta().y / window_size.y * 2.f;
    camY -= io.get_mouse_delta().x / window_size.x * 2.f;
    cam.reset_rotation();
    if (camX > radians(89.9f)) {
        camX = radians(89.9f);
    }
    if (camX < radians(-89.9f)) {
        camX = radians(-89.9f);
    }
    cam.rotate({camX, camY, 0.0f});

    auto velocity_direction = glm::vec3{0.0f};

    if (io.is_pressed(Key::W)) {
        velocity_direction += cam.get_move_direction();
    }

    if (io.is_pressed(Key::S)) {
        velocity_direction -= cam.get_move_direction();
    }

    if (io.is_pressed(Key::D)) {
        velocity_direction += cam.get_right_direction();
    }

    if (io.is_pressed(Key::A)) {
        velocity_direction -= cam.get_right_direction();
    }

    if (io.is_pressed(Key::LeftShift)) {
        velocity_direction.y = -1.0;
    }

    if (velocity_direction != glm::vec3{0.0f}) {
        velocity_direction = glm::normalize(velocity_direction);
    }

    auto prev_velocity = collider->get_collider_velocity();
    auto is_grounded = glm::abs(prev_velocity.y) < 0.01;

    if (is_grounded && io.is_pressed(Key::Space)) {
        velocity_direction.y = 1.0f;
    }

    auto speed = 10.0f;

    if (io.is_pressed(Key::LeftControl)) {
        speed *= 3.0f;
    }

    auto const collider_box = collider->get_collidable_bounding_box();
    auto const camera_pos =
        0.5f *
        (collider_box.hi +
         glm::vec3{collider_box.lo.x, collider_box.hi.y - 0.25f, collider_box.lo.z});

    cam.set_pos(camera_pos);
    prev_velocity.x = prev_velocity.z = 0.0f;
    collider->set_collider_velocity(prev_velocity + speed * velocity_direction);

    {
        auto result = terrain->get_array().ray_cast(
            cam.get_pos(), cam.get_front_direction(), 100.0f
        );

        if (result.has_hit) {
            lineBatch->box(
                glm::vec3{result.voxel_pos} + 0.5f, glm::vec3(1.001f),
                glm::vec4(glm::vec3(60.0f / 255.0f), 0.5f)
            );

            if (io.just_clicked(MouseButton::Left)) {
                terrain->set_voxel(result.voxel_pos, 0);
            }

            if (io.just_clicked(MouseButton::Right)) {
                auto const pos = result.voxel_pos + glm::uvec3{result.normal};
                terrain->set_voxel(pos, (VoxelId) this->currentBlock);
            }
        }
    }
}
