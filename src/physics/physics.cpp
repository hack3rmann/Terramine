#include "../panic.hpp"
#include "../physics.hpp"

namespace tmine {

static auto project(glm::vec3 source, glm::vec3 direction) -> glm::vec3 {
    auto const factor =
        glm::dot(source, direction) / glm::dot(direction, direction);

    return factor * direction;
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
    auto const collision = dynamic_collider->collide(*static_collider);

    if (!collision.exist()) {
        return false;
    }

    auto const displacement = collision.self_displacement;

    auto overlapping_displacement_amount = 0.0f;
    auto free_displacement_amount = 0.0f;
    bool displaced_towards_overlap = false;

    do {
        dynamic_collider->displace_collidable(displacement);
        free_displacement_amount += 1.0f;
    } while (dynamic_collider->collides(*static_collider));

    static auto constexpr MAX_N_STEPS = usize{20};

    for (usize i = 0; i < MAX_N_STEPS; ++i) {
        if (free_displacement_amount - overlapping_displacement_amount <
            accuracy)
        {
            break;
        }

        auto const mid =
            0.5f * (overlapping_displacement_amount + free_displacement_amount);

        auto const displacement_amount =
            displaced_towards_overlap ? mid - overlapping_displacement_amount
                                      : mid - free_displacement_amount;

        dynamic_collider->displace_collidable(
            displacement_amount * displacement
        );

        if (dynamic_collider->collides(*static_collider)) {
            displaced_towards_overlap = true;
            overlapping_displacement_amount = mid;
        } else {
            displaced_towards_overlap = false;
            free_displacement_amount = mid;
        }
    }

    if (displaced_towards_overlap) {
        dynamic_collider->displace_collidable(
            (free_displacement_amount - overlapping_displacement_amount) *
            displacement
        );
    }

    auto const elacticity = glm::max(
        dynamic_collider->collidable_elasticity(),
        static_collider->collidable_elasticity()
    );
    auto const velocity = dynamic_collider->get_collider_velocity();
    auto const parallel_velocity = project(velocity, displacement);
    auto const new_velocity =
        velocity - (1.0f + 1.0f / elacticity) * parallel_velocity;

    dynamic_collider->set_collider_velocity(new_velocity);

    return true;
}

static auto handle_collision(
    RefMut<Collidable> first, RefMut<Collidable> second, Aabb first_box,
    Aabb second_box, f32 accuracy
) -> bool {
    auto first_is_dynamic = first->is_collidable_dynamic();
    auto second_is_dynamic = second->is_collidable_dynamic();

    if (!first_is_dynamic && !second_is_dynamic) {
        return false;
    }

    auto const intersection_box = first_box.intersection(second_box);

    if (intersection_box.is_empty()) {
        return false;
    }

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
        if (!collider->is_collidable_dynamic()) {
            continue;
        }

        auto const acceleration = collider->get_collider_acceleration();
        auto const velocity =
            self.time_step * acceleration + collider->get_collider_velocity();
        auto const displacement = self.time_step * velocity;

        collider->displace_collidable(displacement);
        collider->set_collider_velocity(velocity);
    }

    static auto constexpr MAX_N_STEPS = usize{20};

    for (usize i = 0; i < MAX_N_STEPS; ++i) {
        if (!handle_collisions(self.data, self.accuracy)) {
            break;
        }
    }
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

static auto collide_dynamic_box(
    BoxCollider const& self, BoxCollider const& other
) -> Collision {
    auto const intersection = self.box.intersection(other.box);

    if (intersection.is_empty()) {
        return Collision{};
    }

    auto const size = intersection.hi - intersection.lo;
    auto const self_center = self.box.center();
    auto const other_center = other.box.center();
    auto const to_other_center = other_center - self_center;
    auto const self_volume = self.box.volume();
    auto const other_volume = other.box.volume();
    auto const total_volume = self_volume + other_volume;
    auto const signs = glm::sign(to_other_center);

    if (size.x >= size.y && size.x >= size.z) {
        return Collision{
            -signs.x *
                glm::vec3(size.x * other_volume / total_volume, 0.0f, 0.0f),
            signs.x * glm::vec3(size.x * self_volume / total_volume, 0.0f, 0.0f)
        };
    } else if (size.y >= size.x && size.y >= size.z) {
        return Collision{
            -signs.y *
                glm::vec3(0.0f, size.y * other_volume / total_volume, 0.0f),
            signs.y * glm::vec3(0.0f, size.y * self_volume / total_volume, 0.0f)
        };
    } else {
        return Collision{
            -signs.z *
                glm::vec3(0.0f, 0.0f, size.z * other_volume / total_volume),
            signs.z * glm::vec3(0.0f, 0.0f, size.z * self_volume / total_volume)
        };
    }
}

static auto collide_static_box(
    BoxCollider const& static_box, BoxCollider const& dynamic_box
) -> Collision {
    auto const intersection = static_box.box.intersection(dynamic_box.box);

    if (intersection.is_empty()) {
        return Collision{};
    }

    auto const size = intersection.hi - intersection.lo;
    auto const static_center = static_box.box.center();
    auto const dynamic_center = dynamic_box.box.center();
    auto const signs = glm::sign(dynamic_center - static_center);

    auto displacement = glm::vec3{0.0f};

    if (size.x <= size.y && size.x <= size.z) {
        displacement.x = signs.x * size.x;
    } else if (size.y <= size.x && size.y <= size.z) {
        displacement.y = signs.y * size.y;
    } else {
        displacement.z = signs.z * size.z;
    }

    return Collision{glm::vec3{0.0f}, displacement};
}

auto BoxCollider::collide(Collidable const& other_collider) const -> Collision {
    auto other = dynamic_cast<BoxCollider const*>(&other_collider);

    if (nullptr == other) {
        auto collision = other_collider.collide(*this);
        std::swap(collision.self_displacement, collision.other_displacement);
        return collision;
    }

    auto const self_is_dynamic = this->is_collidable_dynamic();
    auto const other_is_dynamic = other->is_collidable_dynamic();

    auto collision = Collision{};

    if (self_is_dynamic && other_is_dynamic) {
        collision = collide_dynamic_box(*this, *other);
    } else if (!self_is_dynamic && other_is_dynamic) {
        collision = collide_static_box(*this, *other);

        if (collision.self_displacement != glm::vec3(0.0f)) {
            throw Panic("invalid displacement");
        }
    } else if (self_is_dynamic && !other_is_dynamic) {
        collision = collide_static_box(*other, *this);
        std::swap(collision.self_displacement, collision.other_displacement);

        if (collision.other_displacement != glm::vec3(0.0f)) {
            throw Panic("invalid displacement");
        }
    }

    return collision;
}

}  // namespace tmine
