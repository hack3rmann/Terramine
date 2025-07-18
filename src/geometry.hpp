#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "types.hpp"

namespace tmine {

struct Aabb {
    glm::vec3 lo{0.0f};
    glm::vec3 hi{1.0f};

    auto intersection(this Aabb self, Aabb other) -> Aabb;
    auto combination(this Aabb self, Aabb other) -> Aabb;
    auto is_empty(this Aabb self) -> bool;
    auto center(this Aabb self) -> glm::vec3;
    auto size(this Aabb self) -> glm::vec3;
    auto volume(this Aabb self) -> f32;

    inline auto operator==(this Aabb self, Aabb other) -> bool {
        return self.lo == other.lo && self.hi == other.hi;
    }

    inline auto operator!=(this Aabb self, Aabb other) -> bool {
        return !(self == other);
    }
};

auto constexpr INFINITELY_LARGE_AABB = Aabb{glm::vec3{-INFINITY}, glm::vec3{INFINITY}};

}
