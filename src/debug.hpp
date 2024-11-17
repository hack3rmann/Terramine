#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <mutex>
#include <atomic>

#include "controls.hpp"
#include "graphics.hpp"
#include "geometry.hpp"

namespace tmine {

inline constinit auto DEBUG_IS_ENABLED = std::atomic<bool>{false};

struct DebugColor {
    inline static auto constexpr BLACK = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
    inline static auto constexpr WHITE = glm::vec4{1.0f};
    inline static auto constexpr RED = glm::vec4{1.0f, 0.0f, 0.0f, 1.0f};
    inline static auto constexpr GREEN = glm::vec4{0.0f, 1.0f, 0.0f, 1.0f};
    inline static auto constexpr BLUE = glm::vec4{0.0f, 0.0f, 1.0f, 1.0f};
};

class DebugLines {
public:
    DebugLines();

    auto line(
        this DebugLines& self, glm::vec3 from, glm::vec3 to, glm::vec4 color
    ) -> void;

    auto box(
        this DebugLines& self, glm::vec3 position, glm::vec3 size,
        glm::vec4 color = DebugColor::BLUE
    ) -> void;

    auto box(
        this DebugLines& self, Aabb box, glm::vec4 color = DebugColor::BLUE
    ) -> void;

    auto render(
        this DebugLines& self, Camera const& cam, glm::uvec2 viewport_size
    ) -> void;

    inline auto lock(this DebugLines& self) -> void { self.mutex.lock(); }

    inline auto unlock(this DebugLines& self) -> void { self.mutex.unlock(); }

private:
    struct Vertex {
        glm::vec3 pos;
        u32 color;

        static auto constexpr ATTRIBUTE_SIZES = std::array<usize, 2>{3, 1};
    };

private:
    ShaderProgram shader;
    Mesh<Vertex> mesh;
    std::mutex mutex;
};

template <class T>
concept Lockable = requires(T lockable) {
    lockable.lock();
    lockable.unlock();
};

template <Lockable T>
struct Lock {
public:
    inline explicit Lock(T& lockable)
    : lockable{lockable} {
        lockable.lock();
    }

    inline ~Lock() { this->lockable.unlock(); }

    template <class Self>
    inline auto operator->(this Self&& self) -> T* {
        return &self.lockable;
    }

    template <class Self>
    inline auto operator*(this Self&& self) -> T& {
        return self.lockable;
    }

private:
    T& lockable;
};

namespace debug {

    inline constinit auto DEBUG_LINES = std::optional<DebugLines>{};

    inline auto initialize() -> void { DEBUG_LINES.emplace(); }

    inline auto deinitialize() -> void { DEBUG_LINES.reset(); }

    inline auto lines() -> Lock<DebugLines> {
        return Lock{DEBUG_LINES.value()};
    }

    auto update() -> void;

}  // namespace debug

struct DebugOwner {
    bool has_value{true};

    inline DebugOwner() { debug::initialize(); }

    inline DebugOwner(DebugOwner const& other) = delete;

    inline DebugOwner(DebugOwner&& other)
    : has_value{other.has_value} {
        other.has_value = false;
    }

    inline auto operator=(this DebugOwner& self, DebugOwner const& other)
        -> DebugOwner& = delete;

    inline auto operator=(this DebugOwner& self, DebugOwner&& other)
        -> DebugOwner& {
        self.has_value = other.has_value;
        other.has_value = false;
        return self;
    }

    inline ~DebugOwner() {
        if (this->has_value) {
            debug::deinitialize();
        }
    }
};

}  // namespace tmine
