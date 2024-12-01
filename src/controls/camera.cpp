#include <cmath>
#include "../controls.hpp"

namespace tmine {

Camera::Camera()
: Camera{glm::vec3{0.0f}, M_PI / 3.0f} {}

Camera::Camera(glm::vec3 pos, f32 fov)
: pos{pos}
, fov{fov} {}

auto Camera::get_projection(this Camera const& self, f32 aspect_ratio)
    -> glm::mat4 {
    return glm::perspective(
        self.fov, aspect_ratio, Camera::Z_NEAR, Camera::Z_FAR
    );
}

auto Camera::get_view(this Camera const& self) -> glm::mat4 {
    return glm::lookAt(self.pos, self.pos + self.front, Camera::DEFAULT_UP);
}

auto Camera::get_rotation(this Camera const& self) -> glm::mat4 {
    return self.rotation;
}

auto Camera::get_move_direction(this Camera const& self) -> glm::vec3 {
    return glm::normalize(glm::vec3{self.front.x, 0.0f, self.front.z});
}

auto Camera::reset_rotation(this Camera& self) -> void {
    self.rotation = glm::mat4{1.0f};
}

auto Camera::rotate(this Camera& self, glm::vec3 angles) -> void {
    self.rotation =
        glm::rotate(self.rotation, angles.z, glm::vec3{0.0f, 0.0f, 1.0f});

    self.rotation =
        glm::rotate(self.rotation, angles.y, glm::vec3{0.0f, 1.0f, 0.0f});

    self.rotation =
        glm::rotate(self.rotation, angles.x, glm::vec3{1.0f, 0.0f, 0.0f});

    self.front =
        glm::vec3{self.rotation * glm::vec4{Camera::DEFAULT_FRONT, 1.0f}};

    self.right =
        glm::vec3{self.rotation * glm::vec4{Camera::DEFAULT_RIGHT, 1.0f}};
}

}  // namespace tmine
