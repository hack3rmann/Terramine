#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "data.hpp"
#include "physics.hpp"
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

private:
    static auto constexpr DEFAULT_FRONT = glm::vec3{0.0f, 0.0f, -1.0f};
    static auto constexpr DEFAULT_UP = glm::vec3{0.0f, 1.0f, 0.0f};
    static auto constexpr DEFAULT_RIGHT = glm::vec3{1.0f, 0.0f, 0.0f};
    static auto constexpr Z_NEAR = 0.15f;
    static auto constexpr Z_FAR = 1'000.0f;

    glm::vec3 pos;
    glm::vec3 front{DEFAULT_FRONT};
    glm::vec3 right{DEFAULT_RIGHT};
    glm::mat4 rotation{1.0f};
    f32 fov;
};

class Terrain;
class LineBox;

class Player {
public:
    explicit Player(RefMut<PhysicsSolver> solver);

    auto update(
        this Player& self, RefMut<PhysicsSolver> solver,
        RefMut<Terrain> terrain, RefMut<LineBox> selection_box,
        glm::uvec2 window_size
    ) -> void;

    inline auto get_camera(this Player const& self) -> Camera const& {
        return self.camera;
    }

private:
    Camera camera;
    glm::vec2 camera_mouse_angles;
    VoxelId held_voxel_id;
    ColliderId collider_id;
};

}  // namespace tmine
