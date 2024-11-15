#pragma once

#include <vector>
#include <memory>
#include <optional>
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
    auto volume(this Aabb self) -> f32;
};

struct CollidableId {
    u32 value{~u32{0}};

    inline constexpr operator u32() { return value; }
};

struct Collision {
    glm::vec3 self_displacement;
    glm::vec3 other_displacement;
};

inline auto constexpr ABSOLUTELY_ELASTIC_ELASTICITY = 1.0f;
inline auto constexpr ABSOLUTELY_INELASTIC_ELASTICITY = INFINITY;

struct Collidable {
    virtual ~Collidable() = default;

    virtual auto get_collidable_bounding_box() const -> Aabb = 0;
    virtual auto get_collider_velocity() const -> glm::vec3 = 0;
    virtual auto set_collider_velocity(glm::vec3 velocity) -> void = 0;
    virtual auto displace_collidable(glm::vec3 displacement) -> void = 0;
    virtual auto collide(Collidable const& other) const
        -> std::optional<Collision> = 0;

    virtual auto collides(Collidable const& other) const -> bool {
        return this->collide(other).has_value();
    }

    virtual auto is_collidable_dynamic() const -> bool { return true; }

    virtual auto collidable_elasticity() const -> f32 {
        return ABSOLUTELY_ELASTIC_ELASTICITY;
    }
};

class PhysicsSolver {
public:
    inline PhysicsSolver(f32 time_step, f32 accuracy = 0.001f) noexcept
    : time_step{time_step}
    , accuracy{accuracy} {}

    template <std::derived_from<Collidable> T, typename... Args>
    inline auto register_collidable(this PhysicsSolver& self, Args&&... args)
        -> CollidableId {
        auto const id = CollidableId{(u32) self.data.size()};
        self.data.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)
        );

        return id;
    }

    template <std::derived_from<Collidable> T>
    inline auto register_collidable(this PhysicsSolver& self, T collider)
        -> CollidableId {
        return self.register_collidable(std::move(collider));
    }

    auto update(this PhysicsSolver& self, f32 frame_duration) -> void;

private:
    auto fixed_update(this PhysicsSolver& self) -> void;

private:
    f32 time_step;
    f32 previous_frame_reminder{0.0f};
    f32 accuracy;
    std::vector<std::unique_ptr<Collidable>> data{};
};

struct BoxCollider : public Collidable {
    Aabb box;
    glm::vec3 velocity;
    f32 elasticity;

    inline auto get_collidable_bounding_box() const -> Aabb override {
        return this->box;
    }

    inline auto get_collider_velocity() const -> glm::vec3 override {
        return this->velocity;
    }

    inline auto set_collider_velocity(glm::vec3 velocity) -> void override {
        this->velocity = velocity;
    }

    inline auto collidable_elasticity() const -> f32 override {
        return this->elasticity;
    }

    auto displace_collidable(glm::vec3 displacement) -> void override;
    auto collides(Collidable const& other) const -> bool override;
    auto collide(Collidable const& other) const
        -> std::optional<Collision> override;
};

}  // namespace tmine
