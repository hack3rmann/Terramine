#include "../controls.hpp"
#include "../objects.hpp"
#include "../events.hpp"
#include "../debug.hpp"

namespace tmine {

// FIXME(hack3rmann): handle gravity at runtime
#define GRAVITY 1

inline auto constexpr COLLIDER_SIZE = glm::vec3{0.6f, 1.75f, 0.6f};
inline auto constexpr INITIAL_POSITION = glm::vec3{60.0f};

#if GRAVITY
inline auto constexpr GRAVITY_ACCELERATION = glm::vec3{0.0f, -20.0f, 0.0f};
#else
inline auto constexpr GRAVITY_ACCELERATION = glm::vec3{0.0f};
#endif

static auto rotate_camera_by_mouse(
    RefMut<glm::vec2> camera_mouse_angles, RefMut<Camera> camera,
    glm::uvec2 viewport_size
) -> void {
    camera_mouse_angles->x -= io.get_mouse_delta().y / viewport_size.y * 2.f;
    camera_mouse_angles->y -= io.get_mouse_delta().x / viewport_size.x * 2.f;

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

static auto update_movement(RefMut<Camera> camera, RefMut<BoxCollider> collider)
    -> void {
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

    if (velocity_direction != glm::vec3{0.0f}) {
        velocity_direction = glm::normalize(velocity_direction);
    }

    auto prev_velocity = collider->get_collider_velocity();
    auto is_grounded = !GRAVITY || glm::abs(prev_velocity.y) < 0.01;

    auto constexpr SPEED_FALLOFF = 0.7f;

    if (io.is_pressed(Key::LeftShift)) {
        velocity_direction.y = -2.5 / SPEED_FALLOFF;
    }

    if (is_grounded && io.is_pressed(Key::Space)) {
        velocity_direction.y = 2.5f / SPEED_FALLOFF;
    }

    auto speed = SPEED_FALLOFF * 3.0f;

    if (io.is_pressed(Key::LeftControl)) {
        speed *= 3.0f;
    }

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
    prev_velocity.x *= SPEED_FALLOFF;
    prev_velocity.z *= SPEED_FALLOFF;

#if !GRAVITY
    prev_velocity.y = 0.0f;
#endif

    collider->set_collider_velocity(prev_velocity + speed * velocity_direction);
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

        // TODO(hack3rmann): check transparency instead of airness
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

static auto interact_with_terrain(
    RefMut<Terrain> terrain, RefMut<SelectionBox> selection_box,
    Camera const& camera, VoxelId held_voxel_id
) -> void {
    auto ray_cast_result = terrain->get_array().ray_cast(
        camera.get_pos(), camera.get_front_direction(), 100.0f
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

    if (io.just_clicked(MouseButton::Left)) {
        terrain->set_voxel(ray_cast_result.voxel_pos, {});
    }

    auto orientation = u8{0};

    if (ray_cast_result.normal.x != 0.0f) {
        orientation = GameBlock::ORIENTATION_POS_X;
    } else if (ray_cast_result.normal.y != 0.0f) {
        orientation = GameBlock::ORIENTATION_POS_Y;
    } else if (ray_cast_result.normal.z != 0.0f) {
        orientation = GameBlock::ORIENTATION_POS_Z;
    }

    if (io.just_clicked(MouseButton::Right)) {
        auto const pos =
            ray_cast_result.voxel_pos + glm::uvec3{ray_cast_result.normal};
        terrain->set_voxel(pos, {held_voxel_id, Voxel::make_meta(orientation)});
    }
}

Player::Player(RefMut<PhysicsSolver> solver)
: camera{INITIAL_POSITION, glm::radians(60.0f)}
, camera_mouse_angles{glm::vec3{0.0f}}
, held_voxel_id{1}
, collider_id{solver->register_collidable<BoxCollider>(
      Aabb{INITIAL_POSITION, INITIAL_POSITION + COLLIDER_SIZE}, glm::vec3{0.0f},
      GRAVITY_ACCELERATION, ABSOLUTELY_INELASTIC_ELASTICITY
  )} {}

auto Player::update(
    this Player& self, RefMut<PhysicsSolver> solver, RefMut<Terrain> terrain,
    RefMut<SelectionBox> selection_box, glm::uvec2 viewport_size
) -> void {
    auto& collider = solver->get_collidable<BoxCollider>(self.collider_id);

    if (io.is_pressed(Key::P)) {
        collider.box = Aabb{INITIAL_POSITION, INITIAL_POSITION + COLLIDER_SIZE};
        collider.set_collider_velocity(glm::vec3{0.0f});
    }

    rotate_camera_by_mouse(
        &self.camera_mouse_angles, &self.camera, viewport_size
    );

    update_movement(&self.camera, &collider);

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
        terrain, selection_box, self.camera, self.held_voxel_id
    );
}

}  // namespace tmine
