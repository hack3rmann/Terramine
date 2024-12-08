#include "../controls.hpp"
#include "../objects.hpp"
#include "../events.hpp"
#include "../debug.hpp"

namespace tmine {

auto constexpr COLLIDER_SIZE = glm::vec3{0.6f, 1.75f, 0.6f};
auto constexpr INITIAL_POSITION = glm::vec3{60.0f};
auto constexpr GRAVITY_ACCELERATION = glm::vec3{0.0f, -20.0f, 0.0f};
auto constexpr MOUSE_SENSITIVITY = 2.0f;
auto constexpr DEFAULT_FOV = glm::radians(60.0f);
auto constexpr MAX_FOV = glm::radians(120.0f);
auto constexpr MIN_SPEED = 10.0f;

static auto lerp(auto a, auto b, f32 param) {
    return (1.0f - param) * a + param * b;
}

static auto rotate_camera_by_mouse(
    RefMut<glm::vec2> camera_mouse_angles, RefMut<Camera> camera,
    glm::uvec2 viewport_size
) -> void {
    camera_mouse_angles->x -=
        io.get_mouse_delta().y / viewport_size.y * MOUSE_SENSITIVITY;
    camera_mouse_angles->y -=
        io.get_mouse_delta().x / viewport_size.x * MOUSE_SENSITIVITY;

    camera_mouse_angles->x = glm::clamp(
        camera_mouse_angles->x, glm::radians(-89.9f), glm::radians(89.9f)
    );

    camera->reset_rotation();
    camera->rotate({*camera_mouse_angles, 0.0f});
}

static auto pick_new_voxel(RefMut<VoxelId> id) -> void {
    for (u32 i = 0; i < 9; i++) {
        auto key = Key{(u32) Key::Key1 + i};

        if (io.just_pressed(key)) {
            *id = i + 1;
        }
    }

    if (io.just_pressed(Key::Key0)) {
        *id = 10;
    }
}

static auto get_orientation_string(glm::vec3 front) -> std::string_view {
    auto orientation_str = std::string_view{};

    if (glm::abs(front.x) >= glm::abs(front.y) &&
        glm::abs(front.x) >= glm::abs(front.z))
    {
        if (front.x < 0.0f) {
            orientation_str = "Negative X";
        } else {
            orientation_str = "Positive X";
        }
    } else if (glm::abs(front.y) >= glm::abs(front.x) &&
               glm::abs(front.y) >= glm::abs(front.z))
    {
        if (front.y < 0.0f) {
            orientation_str = "Negative Y";
        } else {
            orientation_str = "Positive Y";
        }
    } else {
        if (front.z < 0.0f) {
            orientation_str = "Negative Z";
        } else {
            orientation_str = "Positive Z";
        }
    }

    return orientation_str;
}

static auto update_fov_dynamics(
    f32 time_step, RefMut<Camera> camera, f32 speed,
    RefMut<FovDynamics> dynamics
) -> void {
    auto constexpr ANIMATION_PARAMS = AnimationDynamicsParams{
        .frequency = 3.0f,
        .damping = 2.0f,
        .initial_response = 0.0f,
    };

    auto constexpr POWER = 0.8f;

    auto const confine = [](f32 value) {
        return glm::pow(value / (value + 1.0f), POWER);
    };
    auto const target_fov =
        lerp(DEFAULT_FOV, MAX_FOV, confine(glm::max(0.0f, 0.03f * (speed - MIN_SPEED))));

    auto const target_fov_speed =
        (target_fov - dynamics->prev_target_fov) / time_step;

    auto fov = camera->get_fov();

    ANIMATION_PARAMS.update(
        time_step, &fov, &dynamics->fov_velocity, target_fov, target_fov_speed
    );

    camera->set_fov(fov);
}

static auto update_movement(
    f32 time_step, RefMut<Camera> camera, RefMut<BoxCollider> collider,
    PlayerMovement movement, RefMut<FovDynamics> fov_dynamics,
    RefMut<VelocityDynamics> velocity_dynamics
) -> void {
    auto velocity_direction = glm::vec3{0.0f};

    if (io.is_pressed(Key::W)) {
        velocity_direction += camera->get_move_direction();
    }

    if (io.is_pressed(Key::S)) {
        velocity_direction -= camera->get_move_direction();
    }

    if (io.is_pressed(Key::D)) {
        velocity_direction += camera->get_right_direction();
    }

    if (io.is_pressed(Key::A)) {
        velocity_direction -= camera->get_right_direction();
    }

    if (PlayerMovement::Fly == movement) {
        if (io.is_pressed(Key::Space)) {
            velocity_direction.y += 1.0f;
        }

        if (io.is_pressed(Key::LeftShift)) {
            velocity_direction.y -= 1.0f;
        }

        collider->set_collider_acceleration(glm::vec3{0.0f});
    } else {
        collider->set_collider_acceleration(GRAVITY_ACCELERATION);
    }

    if (velocity_direction != glm::vec3{0.0f}) {
        velocity_direction = glm::normalize(velocity_direction);
    }

    auto speed = PlayerMovement::Walk == movement ? MIN_SPEED : 2.0f * MIN_SPEED;

    if (io.is_pressed(Key::LeftControl)) {
        speed *= 3.0f;
    }

    auto target_velocity = speed * velocity_direction;
    auto prev_velocity = collider->get_collider_velocity();

    auto const collider_box = collider->get_collidable_bounding_box();
    auto const camera_pos =
        0.5f *
        (collider_box.hi +
         glm::vec3{
             collider_box.lo.x, collider_box.hi.y - 0.25f, collider_box.lo.z
         });

    debug::text()->set(
        "camera", fmt::format(
                      "x: {:.2f}, y: {:.2f}, z: {:.2f}", camera_pos.x,
                      camera_pos.y, camera_pos.z
                  )
    );

    debug::text()->set(
        "orientation", fmt::format(
                           "Orientation: {}",
                           get_orientation_string(camera->get_front_direction())
                       )
    );

    camera->set_pos(camera_pos);

    auto const speed_for_fov =
        glm::abs(glm::dot(velocity_direction, camera->get_front_direction())) *
        speed;

    update_fov_dynamics(time_step, camera, speed_for_fov, fov_dynamics);

    auto const velocity_animation_params = PlayerMovement::Walk == movement
        ? AnimationDynamicsParams{
            .frequency = 4.0f,
            .damping = 1.0f,
            .initial_response = 0.0f,
        }
        : AnimationDynamicsParams{
            .frequency = 10.0,
            .damping = 5.0,
            .initial_response = 0.0f,
        };

    auto next_velocity = prev_velocity;

    if (PlayerMovement::Fly == movement) {
        velocity_animation_params.update(
            time_step, &next_velocity,
            &velocity_dynamics->velocity_rate_of_change, target_velocity,
            velocity_dynamics->target_velocity_rate_of_change
        );
    } else {
        auto flat_next_velocity = glm::vec2{next_velocity.x, next_velocity.z};
        auto flat_velocity_rate_of_change = glm::vec2{
            velocity_dynamics->velocity_rate_of_change.x,
            velocity_dynamics->velocity_rate_of_change.z
        };
        auto const flat_velocity =
            glm::vec2{target_velocity.x, target_velocity.z};
        auto const flat_target_velocity_rate_of_change = glm::vec2{
            velocity_dynamics->target_velocity_rate_of_change.x,
            velocity_dynamics->target_velocity_rate_of_change.z
        };

        velocity_animation_params.update(
            time_step, &flat_next_velocity, &flat_velocity_rate_of_change,
            flat_velocity, flat_target_velocity_rate_of_change
        );

        next_velocity = glm::vec3{
            flat_next_velocity.x, next_velocity.y, flat_next_velocity.y
        };
        velocity_dynamics->velocity_rate_of_change = glm::vec3{
            flat_velocity_rate_of_change.x, 0.0f, flat_velocity_rate_of_change.y
        };
    }

    if (PlayerMovement::Walk == movement) {
        auto const is_grounded =
            PlayerMovement::Fly == movement || glm::abs(prev_velocity.y) < 0.01;

        if (is_grounded && io.is_pressed(Key::Space)) {
            next_velocity.y = 0.7f * speed;
        }
    }

    collider->set_collider_velocity(next_velocity);
}

static auto draw_selection_box(
    RefMut<SelectionBox> selection_box, glm::uvec3 voxel_position,
    glm::vec3 camera_pos, Terrain const& terrain
) -> void {
    auto constexpr TIGHTEN_OFFSET = 0.0001f;

    auto const position = glm::vec3{voxel_position};

    auto box =
        Aabb{position + TIGHTEN_OFFSET, position + 1.0f - TIGHTEN_OFFSET};

    auto const blocked_from = [&](glm::ivec3 offset) {
        auto maybe_voxel =
            terrain.get_array().get_voxel(glm::ivec3(position) + offset);

        return maybe_voxel.has_value() && 0 != maybe_voxel->id;
    };

    auto const face_is_visible = [&](glm::vec3 offset) {
        auto const face_pos = box.center() + 0.5f * offset;
        return glm::dot(face_pos - camera_pos, offset) <= 0.0f;
    };

    auto sides = 0u;

    if (!blocked_from({-1, 0, 0}) && face_is_visible({-1, 0, 0})) {
        sides |= Side::NEG_X;
    }

    if (!blocked_from({0, -1, 0}) && face_is_visible({0, -1, 0})) {
        sides |= Side::NEG_Y;
    }

    if (!blocked_from({0, 0, -1}) && face_is_visible({0, 0, -1})) {
        sides |= Side::NEG_Z;
    }

    if (!blocked_from({1, 0, 0}) && face_is_visible({1, 0, 0})) {
        sides |= Side::POS_X;
    }

    if (!blocked_from({0, 1, 0}) && face_is_visible({0, 1, 0})) {
        sides |= Side::POS_Y;
    }

    if (!blocked_from({0, 0, 1}) && face_is_visible({0, 0, 1})) {
        sides |= Side::POS_Z;
    }

    selection_box->box(box, glm::vec4{glm::vec3{1.0f}, 0.7f}, sides);
}

// FIXME(hack3rmann): make a runtime mode 'excavate'
#define EXCAVATE 0
#if EXCAVATE
#    define clicked is_clicked
#else
#    define clicked just_clicked
#endif

static auto interact_with_terrain(
    RefMut<Terrain> terrain, RefMut<SelectionBox> selection_box,
    BoxCollider const& player_collider, Camera const& camera,
    VoxelId held_voxel_id
) -> void {
    auto constexpr MAX_DISTANCE = 1000.0f;

    auto ray_cast_result = terrain->get_array().ray_cast(
        camera.get_pos(), camera.get_front_direction(), MAX_DISTANCE
    );

    if (!ray_cast_result.has_hit) {
        selection_box->clear();
        debug::text()->set("look_on", "Look on 'air'");

        return;
    }

    draw_selection_box(
        selection_box, ray_cast_result.voxel_pos, camera.get_pos(), *terrain
    );

    auto const look_voxel =
        terrain->get_array().get_voxel(ray_cast_result.voxel_pos).value();

    debug::text()->set(
        "look_on", fmt::format(
                       "Look on '{}'",
                       terrain->get_data()
                           .get_block(look_voxel.id, look_voxel.orientation())
                           .name
                   )
    );

    if (io.clicked(MouseButton::Left)) {
        terrain->set_voxel(ray_cast_result.voxel_pos, {});

#if EXCAVATE
        auto constexpr RADIUS = i32{15};

        for (i32 x = -RADIUS; x <= RADIUS; ++x) {
            for (i32 y = -RADIUS; y <= RADIUS; ++y) {
                for (i32 z = -RADIUS; z <= RADIUS; ++z) {
                    auto const pos = glm::vec3{x, y, z};

                    if (glm::dot(pos, pos) <= RADIUS * RADIUS) {
                        terrain->set_voxel(
                            glm::ivec3{ray_cast_result.voxel_pos} +
                                glm::ivec3{x, y, z},
                            {}
                        );
                    }
                }
            }
        }
#endif
    }

    auto orientation = u8{0};

    if (ray_cast_result.normal.x != 0.0f) {
        orientation = GameBlock::ORIENTATION_POS_X;
    } else if (ray_cast_result.normal.y != 0.0f) {
        orientation = GameBlock::ORIENTATION_POS_Y;
    } else if (ray_cast_result.normal.z != 0.0f) {
        orientation = GameBlock::ORIENTATION_POS_Z;
    }

    auto const new_voxel_pos =
        ray_cast_result.voxel_pos + glm::uvec3{ray_cast_result.normal};

    auto const new_voxel_box =
        Aabb{new_voxel_pos, glm::vec3{new_voxel_pos} + glm::vec3{1.0f}};

    auto constexpr PUSHOUT_THREASHOLD = 0.5f;

    auto const intersection_size =
        player_collider.box.intersection(new_voxel_box).size();

    auto const player_is_pushable =
        std::min({intersection_size.x, intersection_size.y, intersection_size.z}
        ) < PUSHOUT_THREASHOLD;

    if (io.clicked(MouseButton::Right) && player_is_pushable) {
        terrain->set_voxel(
            new_voxel_pos, {held_voxel_id, Voxel::make_meta(orientation)}
        );

#if EXCAVATE
        auto constexpr RADIUS = i32{10};

        for (i32 x = -RADIUS; x <= RADIUS; ++x) {
            for (i32 y = -RADIUS; y <= RADIUS; ++y) {
                for (i32 z = -RADIUS; z <= RADIUS; ++z) {
                    auto const pos = glm::vec3{x, y, z};

                    if (glm::dot(pos, pos) <= RADIUS * RADIUS) {
                        terrain->set_voxel(
                            glm::ivec3{ray_cast_result.voxel_pos} +
                                glm::ivec3{x, y, z},
                            {held_voxel_id, Voxel::make_meta(orientation)}
                        );
                    }
                }
            }
        }
#endif
    }
}

static auto reset_collider(Terrain const& terrain, RefMut<BoxCollider> collider)
    -> void {
    auto const world_size = terrain.get_array().size() * Chunk::SIZE;
    auto surface_center = glm::uvec3{
        world_size.x / 2,
        world_size.y,
        world_size.z / 2,
    };

    // TODO(hack3rmann): check all voxels that may hit player's collider
    for (; surface_center.y != 0; --surface_center.y) {
        auto voxel = terrain.get_array().get_voxel(surface_center);

        if (voxel.has_value() && voxel->id != 0) {
            break;
        }
    }

    if (surface_center.y != 0) {
        surface_center.y += 1;
    } else {
        surface_center.y = world_size.y;
    }

    collider->box =
        Aabb{surface_center, glm::vec3{surface_center} + COLLIDER_SIZE};
    collider->set_collider_velocity(glm::vec3{0.0f});
}

static auto spawn_collider(Terrain const& terrain, RefMut<PhysicsSolver> solver)
    -> ColliderId {
    auto collider = BoxCollider{
        Aabb{INITIAL_POSITION, INITIAL_POSITION + COLLIDER_SIZE},
        glm::vec3{0.0f},
        GRAVITY_ACCELERATION,
        ABSOLUTELY_INELASTIC_ELASTICITY,
    };

    reset_collider(terrain, &collider);

    return solver->register_collidable<BoxCollider>(std::move(collider));
}

Player::Player(Terrain const& terrain, RefMut<PhysicsSolver> solver)
: collider_id{spawn_collider(terrain, solver)}
, camera{INITIAL_POSITION, glm::radians(60.0f)}
, camera_mouse_angles{glm::vec3{0.0f}}
, held_voxel_id{1} {}

auto Player::fixed_update(
    this Player& self, f32 time_step, RefMut<PhysicsSolver> solver
) -> void {
    auto& collider = solver->get_collidable<BoxCollider>(self.collider_id);

    update_movement(
        time_step, &self.camera, &collider, self.movement, &self.fov_dynamics,
        &self.velocity_dynamics
    );
}

auto Player::update(
    this Player& self, RefMut<PhysicsSolver> solver, RefMut<Terrain> terrain,
    RefMut<SelectionBox> selection_box, glm::uvec2 viewport_size
) -> void {
    auto& collider = solver->get_collidable<BoxCollider>(self.collider_id);

    if (io.is_pressed(Key::P)) {
        reset_collider(*terrain, &collider);
    }

    if (io.just_pressed(Key::F)) {
        if (self.movement == PlayerMovement::Walk) {
            self.movement = PlayerMovement::Fly;
        } else {
            self.movement = PlayerMovement::Walk;
        }
    }

    rotate_camera_by_mouse(
        &self.camera_mouse_angles, &self.camera, viewport_size
    );

    debug::text()->set(
        "hold", fmt::format(
                    "Held Block: {}",
                    terrain->get_data()
                        .get_block(self.held_voxel_id, Orientation::PosX)
                        .name
                )
    );

    pick_new_voxel(&self.held_voxel_id);
    interact_with_terrain(
        terrain, selection_box, collider, self.camera, self.held_voxel_id
    );
}

}  // namespace tmine
