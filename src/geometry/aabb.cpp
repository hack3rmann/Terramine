#include "../geometry.hpp"

namespace tmine {

auto Aabb::intersection(this Aabb self, Aabb other) -> Aabb {
    return Aabb{
        .lo = glm::max(self.lo, other.lo),
        .hi = glm::min(self.hi, other.hi),
    };
}

auto Aabb::combination(this Aabb self, Aabb other) -> Aabb {
    return Aabb{
        .lo = glm::min(self.lo, other.lo),
        .hi = glm::max(self.hi, other.hi),
    };
}

auto Aabb::is_empty(this Aabb self) -> bool {
    return self.lo.x >= self.hi.x || self.lo.y >= self.hi.y ||
           self.lo.z >= self.hi.z;
}

auto Aabb::center(this Aabb self) -> glm::vec3 {
    return 0.5f * (self.lo + self.hi);
}

auto Aabb::size(this Aabb self) -> glm::vec3 { return self.hi - self.lo; }

auto Aabb::volume(this Aabb self) -> f32 {
    auto const difference = self.hi - self.lo;
    return difference.x * difference.y * difference.z;
}

}  // namespace tmine
