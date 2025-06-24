#include "../physics.hpp"
#include <span>

namespace tmine {

static auto project(glm::vec3 source, glm::vec3 direction) -> glm::vec3 {
    auto const factor =
        glm::dot(source, direction) / glm::dot(direction, direction);

    return factor * direction;
}

static auto displace(
    RefMut<Collidable> first_collider, RefMut<Collidable> second_collider
) -> bool {
    auto const collision = first_collider->collide(*second_collider);

    if (!collision.exist()) {
        return false;
    }

    auto const first_displacement = collision.self_displacement;
    auto const second_displacement = collision.other_displacement;

    first_collider->displace_collidable(first_displacement);
    second_collider->displace_collidable(second_displacement);

    auto const elacticity = glm::min(
        first_collider->collidable_elasticity(),
        second_collider->collidable_elasticity()
    );

    auto elastic_collision_velocity =
        [](f32 first_mass, f32 second_mass, glm::vec3 first_velocity,
           glm::vec3 second_velocity) -> glm::vec3 {
        return ((first_mass - second_mass) * first_velocity +
                2.0f * second_mass * second_velocity) /
               (first_mass + second_mass);
    };

    auto const first_velocity = first_collider->get_collider_velocity();
    auto const first_parallel_velocity =
        project(first_velocity, first_displacement);

    auto const second_velocity = second_collider->get_collider_velocity();
    auto const second_parallel_velocity =
        project(second_velocity, second_displacement);

    auto const first_mass =
        first_collider->get_collidable_bounding_box().volume();
    auto const second_mass =
        second_collider->get_collidable_bounding_box().volume();

    auto const first_new_velocity =
        first_velocity + (1.0f + elacticity) * elastic_collision_velocity(
                                                   first_mass, second_mass,
                                                   first_parallel_velocity,
                                                   second_parallel_velocity
                                               );

    auto const second_new_velocity =
        second_velocity + (1.0f + elacticity) * elastic_collision_velocity(
                                                    second_mass, first_mass,
                                                    second_parallel_velocity,
                                                    first_parallel_velocity
                                                );

    first_collider->set_collider_velocity(first_new_velocity);
    second_collider->set_collider_velocity(second_new_velocity);

    return true;
}

static auto static_binary_displace(
    RefMut<Collidable> static_collider, RefMut<Collidable> dynamic_collider
) -> bool {
    auto const collision = dynamic_collider->collide(*static_collider);

    if (!collision.exist()) {
        return false;
    }

    auto const displacement = collision.self_displacement;

    dynamic_collider->displace_collidable(displacement);

    auto const elacticity = glm::min(
        dynamic_collider->collidable_elasticity(),
        static_collider->collidable_elasticity()
    );
    auto const velocity = dynamic_collider->get_collider_velocity();
    auto const parallel_velocity = project(velocity, displacement);
    auto const new_velocity =
        velocity - (1.0f + elacticity) * parallel_velocity;

    dynamic_collider->set_collider_velocity(new_velocity);

    return true;
}

static auto handle_collision(
    RefMut<Collidable> first, RefMut<Collidable> second, Aabb first_box,
    Aabb second_box
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
        return displace(first, second);
    } else if (!first_is_dynamic && second_is_dynamic) {
        return static_binary_displace(first, second);
    } else if (first_is_dynamic && !second_is_dynamic) {
        return static_binary_displace(second, first);
    } else {
        return false;
    }
}

static auto handle_collisions(
    std::span<std::unique_ptr<Collidable>> collidables
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
                second_box
            );
        }
    }

    return do_any_collide;
}

auto PhysicsSolver::update(this PhysicsSolver& self, f32 time_step)
    -> void {
    for (auto& collider : self.colliders) {
        if (!collider->is_collidable_dynamic()) {
            continue;
        }

        auto const acceleration = collider->get_collider_acceleration();
        auto const velocity =
            time_step * acceleration + collider->get_collider_velocity();
        auto const displacement = time_step * velocity;

        collider->displace_collidable(displacement);
        collider->set_collider_velocity(velocity);
    }

    for (usize i = 0; i < MAX_N_DISPLACE_STEPS; ++i) {
        if (!handle_collisions(self.colliders)) {
            break;
        }
    }
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

    if (size.x <= size.y && size.x <= size.z) {
        return Collision{
            -signs.x *
                glm::vec3(size.x * other_volume / total_volume, 0.0f, 0.0f),
            signs.x * glm::vec3(size.x * self_volume / total_volume, 0.0f, 0.0f)
        };
    } else if (size.y <= size.x && size.y <= size.z) {
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
    } else if (size.z <= size.x && size.z <= size.y) {
        displacement.z = signs.z * size.z;
    } else {
        displacement.y = signs.y * size.y;
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
    } else if (self_is_dynamic && !other_is_dynamic) {
        collision = collide_static_box(*other, *this);
        std::swap(collision.self_displacement, collision.other_displacement);
    }

    return collision;
}

}  // namespace tmine
