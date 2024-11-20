#include "../controls.hpp"
#include "../objects.hpp"
#include "../events.hpp"

namespace tmine {

inline auto constexpr COLLIDER_SIZE = glm::vec3{0.6f, 1.75f, 0.6f};
inline auto constexpr GRAVITY_ACCELERATION = glm::vec3{0.0f, -20.0f, 0.0f};
inline auto constexpr INITIAL_POSITION = glm::vec3{60.0f};

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
         glm::vec3{
             collider_box.lo.x, collider_box.hi.y - 0.25f, collider_box.lo.z
         });

    camera->set_pos(camera_pos);
    prev_velocity.x = prev_velocity.z = 0.0f;
    collider->set_collider_velocity(prev_velocity + speed * velocity_direction);
}

static auto interact_with_terrain(
    RefMut<Terrain> terrain, RefMut<LineBox> selection_box,
    Camera const& camera, VoxelId held_voxel_id
) -> void {
    auto ray_cast_result = terrain->get_array().ray_cast(
        camera.get_pos(), camera.get_front_direction(), 100.0f
    );

    if (!ray_cast_result.has_hit) {
        selection_box->clear();
        return;
    }

    selection_box->box(
        glm::vec3{ray_cast_result.voxel_pos} + 0.5f, glm::vec3{1.001f},
        glm::vec4{glm::vec3{60.0f / 255.0f}, 0.5f}
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
    RefMut<LineBox> selection_box, glm::uvec2 viewport_size
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

    pick_new_voxel(&self.held_voxel_id);
    interact_with_terrain(
        terrain, selection_box, self.camera, self.held_voxel_id
    );
}

}  // namespace tmine
