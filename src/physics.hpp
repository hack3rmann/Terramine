#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "types.hpp"
#include "panic.hpp"
#include "geometry.hpp"

namespace tmine {

struct ColliderId {
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
inline auto constexpr ABSOLUTELY_INELASTIC_ELASTICITY = 0.0f;

struct Collidable {
    virtual ~Collidable() = default;

    virtual auto get_collidable_bounding_box() const -> Aabb = 0;
    virtual auto get_collider_velocity() const -> glm::vec3 = 0;
    virtual auto set_collider_velocity(glm::vec3 velocity) -> void = 0;

    inline virtual auto get_collider_acceleration() const -> glm::vec3 {
        return glm::vec3{0.0f};
    }

    inline virtual auto set_collider_acceleration(glm::vec3) -> void {}

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
    PhysicsSolver() noexcept = default;

    template <std::derived_from<Collidable> T, typename... Args>
    inline auto register_collidable(this PhysicsSolver& self, Args&&... args)
        -> ColliderId {
        auto const id = ColliderId{(u32) self.colliders.size()};
        self.colliders.emplace_back(std::make_unique<T>(std::forward<Args>(args
        )...));

        return id;
    }

    template <std::derived_from<Collidable> T>
    inline auto register_collidable(this PhysicsSolver& self, T collider)
        -> ColliderId {
        return self.register_collidable(std::move(collider));
    }

    template <std::derived_from<Collidable> T, class Self>
    inline auto get_collidable(this Self&& self, ColliderId id)
        -> decltype(auto) {
        if (id >= self.colliders.size()) {
            throw Panic("invalid collidable id {}", id.value);
        }

        auto result = dynamic_cast<T*>(self.colliders[(usize) id.value].get());

        if (nullptr == result) {
            throw Panic(
                "invalid type `{}` for collidable id {}", typeid(T).name(),
                id.value
            );
        }

        return std::forward_like<Self>(*result);
    }

    auto update(this PhysicsSolver& self, f32 time_step) -> void;

private:
    std::vector<std::unique_ptr<Collidable>> colliders{};

    static auto constexpr MAX_N_DISPLACE_STEPS = usize{20};
};

struct BoxCollider : public Collidable {
    Aabb box;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    f32 elasticity;
    bool is_dynamic;

    inline BoxCollider(
        Aabb box = Aabb{}, glm::vec3 velocity = glm::vec3{0.0f},
        glm::vec3 acceleration = glm::vec3{0.0f},
        f32 elasticity = ABSOLUTELY_ELASTIC_ELASTICITY, bool is_dynamic = true
    )
    : box{box}
    , velocity{velocity}
    , acceleration{acceleration}
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

    inline auto get_collider_acceleration() const -> glm::vec3 override {
        return this->acceleration;
    }

    inline auto set_collider_acceleration(glm::vec3 value) -> void override {
        this->acceleration = value;
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
