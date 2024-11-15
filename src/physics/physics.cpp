#include "../panic.hpp"
#include "../physics.hpp"

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

auto Aabb::volume(this Aabb self) -> f32 {
    auto const difference = self.hi - self.lo;
    return difference.x * difference.y * difference.z;
}

auto PhysicsSolver::update(this PhysicsSolver& self, f32 frame_duration)
    -> void {
    auto duration = self.previous_frame_reminder + frame_duration;

    while (self.time_step < duration) {
        duration -= self.time_step;
        self.fixed_update();
    }

    self.previous_frame_reminder = duration;
}

static auto binary_displace(
    [[maybe_unused]] RefMut<Collidable> first_collider,
    [[maybe_unused]] RefMut<Collidable> second_collider,
    [[maybe_unused]] f32 accuracy
) -> bool {
    throw Unimplemented();
}

static auto static_binary_displace(
    RefMut<Collidable> static_collider, RefMut<Collidable> dynamic_collider,
    f32 accuracy
) -> bool {
    auto maybe_collision = dynamic_collider->collide(*static_collider);

    if (!maybe_collision.has_value()) {
        return false;
    }

    auto const collision = std::move(maybe_collision).value();
    auto const displacement = collision.self_displacement;

    auto overlapping_displacement_amount = 0.0f;
    auto free_displacement_amount = 0.0f;
    bool displaced_towards_overlap = false;

    do {
        dynamic_collider->displace_collidable(displacement);
        free_displacement_amount += 1.0f;
    } while (dynamic_collider->collides(*static_collider));

    while (free_displacement_amount - overlapping_displacement_amount >=
           accuracy)
    {
        auto const mid =
            0.5f * (overlapping_displacement_amount + free_displacement_amount);

        auto const displacement_amount =
            displaced_towards_overlap ? mid - overlapping_displacement_amount
                                      : mid - free_displacement_amount;

        dynamic_collider->displace_collidable(
            displacement_amount * displacement
        );

        if (dynamic_collider->collides(*static_collider)) {
            overlapping_displacement_amount = mid;
        } else {
            free_displacement_amount = mid;
        }
    }

    if (displaced_towards_overlap) {
        dynamic_collider->displace_collidable(
            (free_displacement_amount - overlapping_displacement_amount) *
            displacement
        );
    }

    return true;
}

static auto handle_collision(
    RefMut<Collidable> first, RefMut<Collidable> second, Aabb first_box,
    Aabb second_box, f32 accuracy
) -> bool {
    if (!first->is_collidable_dynamic() && !second->is_collidable_dynamic()) {
        return false;
    }

    auto const intersection_box = first_box.intersection(second_box);

    if (intersection_box.is_empty()) {
        return false;
    }

    auto first_is_dynamic = first->is_collidable_dynamic();
    auto second_is_dynamic = second->is_collidable_dynamic();

    if (first_is_dynamic && second_is_dynamic) {
        return binary_displace(first, second, accuracy);
    } else if (!first_is_dynamic && second_is_dynamic) {
        return static_binary_displace(first, second, accuracy);
    } else if (first_is_dynamic && !second_is_dynamic) {
        return static_binary_displace(second, first, accuracy);
    } else {
        return false;
    }
}

static auto handle_collisions(
    std::span<std::unique_ptr<Collidable>> collidables, f32 accuracy
) -> bool {
    auto do_any_collide = false;

    for (usize i = 0; i < collidables.size(); ++i) {
        auto& first_collider = collidables[i];
        auto const first_box = first_collider->get_collidable_bounding_box();

        for (usize j = i + 1; j < collidables.size(); ++j) {
            auto& second_collider = collidables[j];
            auto const second_box =
                second_collider->get_collidable_bounding_box();

            handle_collision(
                first_collider.get(), second_collider.get(), first_box,
                second_box, accuracy
            );
        }
    }

    return do_any_collide;
}

auto PhysicsSolver::fixed_update(this PhysicsSolver& self) -> void {
    for (auto& collider : self.data) {
        auto const displacement =
            self.time_step * collider->collider_velocity();
        collider->displace_collidable(displacement);
    }

    while (handle_collisions(self.data, self.accuracy)) {
    }
}

auto BoxCollider::get_collidable_bounding_box() const -> Aabb {
    return this->box;
}

auto BoxCollider::collider_velocity() const -> glm::vec3 {
    return this->velocity;
}

auto BoxCollider::displace_collidable(glm::vec3 displacement) -> void {
    this->box.lo += displacement;
    this->box.hi += displacement;
}

auto BoxCollider::collides(Collidable const& other) const -> bool {
    if (auto box_collider = dynamic_cast<BoxCollider const*>(&other)) {
        return !this->box.intersection(box_collider->box).is_empty();
    } else {
        return other.collides(*this);
    }
}

auto BoxCollider::collide(Collidable const& other) const
    -> std::optional<Collision> {
    auto other_collider = dynamic_cast<BoxCollider const*>(&other);

    if (nullptr == other_collider) {
        return other.collide(*this);
    }

    auto const intersection = this->box.intersection(other_collider->box);

    if (intersection.is_empty()) {
        return std::nullopt;
    }

    auto const size = intersection.hi - intersection.lo;
    auto const self_volume = this->box.volume();
    auto const other_volume = other_collider->box.volume();
    auto const total_volume = self_volume + other_volume;

    if (size.x >= size.y && size.x >= size.z) {
        return std::make_optional<Collision>(
            glm::vec3(size.x * other_volume / total_volume, 0.0f, 0.0f),
            glm::vec3(size.x * self_volume / total_volume, 0.0f, 0.0f)
        );
    } else if (size.y >= size.x && size.y >= size.z) {
        return std::make_optional<Collision>(
            glm::vec3(0.0f, size.y * other_volume / total_volume, 0.0f),
            glm::vec3(0.0f, size.y * self_volume / total_volume, 0.0f)
        );
    } else {
        return std::make_optional<Collision>(
            glm::vec3(0.0f, 0.0f, size.z * other_volume / total_volume),
            glm::vec3(0.0f, 0.0f, size.z * self_volume / total_volume)
        );
    }
}

}  // namespace tmine
