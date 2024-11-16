#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "types.hpp"
#include "panic.hpp"
#include "geometry.hpp"

namespace tmine {

struct CollidableId {
    u32 value{~u32{0}};

    inline constexpr operator u32() { return value; }
};

struct Collision {
    glm::vec3 self_displacement{0.0f};
    glm::vec3 other_displacement{0.0f};

    inline auto exist(this Collision const& self) -> bool {
        return self.self_displacement != glm::vec3{0.0f} ||
               self.other_displacement != glm::vec3{0.0f};
    }
};

inline auto constexpr ABSOLUTELY_ELASTIC_ELASTICITY = 1.0f;
inline auto constexpr ABSOLUTELY_INELASTIC_ELASTICITY = INFINITY;

struct Collidable {
    virtual ~Collidable() = default;

    virtual auto get_collidable_bounding_box() const -> Aabb = 0;
    virtual auto get_collider_velocity() const -> glm::vec3 = 0;
    virtual auto set_collider_velocity(glm::vec3 velocity) -> void = 0;
    virtual auto displace_collidable(glm::vec3 displacement) -> void = 0;
    virtual auto collide(Collidable const& other) const -> Collision = 0;

    inline virtual auto collides(Collidable const& other) const -> bool {
        return this->collide(other).exist();
    }

    inline virtual auto is_collidable_dynamic() const -> bool { return true; }

    inline virtual auto collidable_elasticity() const -> f32 {
        return ABSOLUTELY_ELASTIC_ELASTICITY;
    }
};

class PhysicsSolver {
public:
    inline PhysicsSolver(
        f32 time_step = 1.0f / 60.0f / 20.0f, f32 accuracy = 0.0001f
    ) noexcept
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

    template <std::derived_from<Collidable> T, class Self>
    inline auto get_collidable(this Self&& self, CollidableId id)
        -> decltype(auto) {
        if (id >= self.data.size()) {
            throw Panic("invalid collidable id {}", id.value);
        }

        auto result = dynamic_cast<T*>(self.data[(usize) id.value].get());

        if (nullptr == result) {
            throw Panic(
                "invalid type `{}` for collidable id {}", typeid(T).name(),
                id.value
            );
        }

        return std::forward_like<Self>(*result);
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
    bool is_dynamic;

    inline BoxCollider(
        Aabb box = Aabb{}, glm::vec3 velocity = glm::vec3{0.0f},
        f32 elasticity = ABSOLUTELY_ELASTIC_ELASTICITY, bool is_dynamic = true
    )
    : box{box}
    , velocity{velocity}
    , elasticity{elasticity}
    , is_dynamic{is_dynamic} {}

    inline auto is_collidable_dynamic() const -> bool override {
        return this->is_dynamic;
    }

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
    auto collide(Collidable const& other) const -> Collision override;
};

}  // namespace tmine
