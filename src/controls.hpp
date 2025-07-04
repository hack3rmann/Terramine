#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "physics.hpp"
#include "terrain.hpp"
#include "types.hpp"

namespace tmine {

class Camera {
public:
    Camera();
    Camera(glm::vec3 pos, f32 fov);

    auto reset_rotation(this Camera& self) -> void;
    auto rotate(this Camera& self, glm::vec3 angles) -> void;
    auto get_projection(this Camera const& self, f32 aspect_ratio) -> glm::mat4;
    auto get_view(this Camera const& self) -> glm::mat4;
    auto get_rotation(this Camera const& self) -> glm::mat4;

    auto get_move_direction(this Camera const& self) -> glm::vec3;

    inline auto get_pos(this Camera const& self) -> glm::vec3 {
        return self.pos;
    }

    inline auto set_pos(this Camera& self, glm::vec3 pos) -> void {
        self.pos = pos;
    }

    inline auto displace(this Camera& self, glm::vec3 offset) -> void {
        self.pos += offset;
    }

    inline auto get_front_direction(this Camera const& self) -> glm::vec3 {
        return self.front;
    }

    inline auto get_right_direction(this Camera const& self) -> glm::vec3 {
        return self.right;
    }

    inline auto get_up_direction([[maybe_unused]] this Camera const& self)
        -> glm::vec3 {
        return Camera::DEFAULT_UP;
    }

    inline auto get_fov(this Camera const& self) -> f32 { return self.fov; }

    inline auto set_fov(this Camera& self, f32 value) -> void {
        self.fov = value;
    }

private:
    static auto constexpr DEFAULT_FRONT = glm::vec3{0.0f, 0.0f, -1.0f};
    static auto constexpr DEFAULT_UP = glm::vec3{0.0f, 1.0f, 0.0f};
    static auto constexpr DEFAULT_RIGHT = glm::vec3{1.0f, 0.0f, 0.0f};
    static auto constexpr Z_NEAR = 0.01f;
    static auto constexpr Z_FAR = 1'000.0f;

    glm::vec3 pos;
    glm::vec3 front{DEFAULT_FRONT};
    glm::vec3 right{DEFAULT_RIGHT};
    glm::mat4 rotation{1.0f};
    f32 fov;
};

class Terrain;
class SelectionBox;

enum class PlayerMovement { Walk = 0, Fly };

struct FovDynamics {
    f32 fov_velocity{0.0f};
    f32 prev_target_fov{glm::radians(60.0f)};
};

struct VelocityDynamics {
    glm::vec3 target_velocity_rate_of_change{0.0f};
    glm::vec3 velocity_rate_of_change{0.0f};
};

class Player {
public:
    explicit Player(Terrain const& terrain, RefMut<PhysicsSolver> solver);

    auto update(
        this Player& self, RefMut<PhysicsSolver> solver,
        RefMut<Terrain> terrain, RefMut<SelectionBox> selection_box,
        glm::uvec2 window_size
    ) -> void;

    auto fixed_update(
        this Player& self, f32 time_step, Terrain const& terrain,
        RefMut<PhysicsSolver> solver
    ) -> void;

    inline auto get_camera(this Player const& self) -> Camera const& {
        return self.camera;
    }

private:
    ColliderId collider_id;
    Camera camera;
    FovDynamics fov_dynamics;
    VelocityDynamics velocity_dynamics;
    PlayerMovement movement{PlayerMovement::Walk};
    glm::vec2 camera_mouse_angles;
    VoxelId held_voxel_id;
};

}  // namespace tmine
