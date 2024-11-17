#pragma once

#include <fmt/printf.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "types.hpp"

namespace tmine {

enum class CursorMode : u32 {
    Normal = GLFW_CURSOR_NORMAL,
    Hidden = GLFW_CURSOR_HIDDEN,
    Disabled = GLFW_CURSOR_DISABLED,
    Captured = GLFW_CURSOR_CAPTURED,
};

struct WindowData {
    glm::uvec2 size;
    bool is_cursor_locked;
};

class Window {
public:
    explicit Window(
        char const* title, u32 width = DEFAULT_WIDTH,
        u32 height = DEFAULT_HEIGHT
    );

    ~Window();
    Window(Window const&) = delete;
    Window(Window&& other);
    auto operator=(this Window&, Window const&) = delete;
    auto operator=(this Window& self, Window&& other) -> Window&;

    static auto aspect_ratio_of(glm::uvec2 size) noexcept -> f32;

    inline static auto is_visible(glm::uvec2 viewport_size) noexcept -> bool {
        return 0 != viewport_size.x && 0 != viewport_size.y;
    }

    inline auto get_glfw_window(this Window& self) noexcept -> GLFWwindow* {
        return self.glfw_window;
    }

    inline auto size(this Window const& self) noexcept -> glm::uvec2 {
        return self.data->size;
    }

    inline auto get_aspect_ratio(this Window const& self) noexcept -> f32 {
        return Window::aspect_ratio_of(self.size());
    }

    auto is_open(this Window const& self) noexcept -> bool;
    auto schedule_close(this Window const& self) noexcept -> void;
    auto swap_buffers(this Window const& self) noexcept -> void;
    auto toggle_cursor_visibility(this Window& self) -> void;
    auto set_cursor_mode(this Window const& self, CursorMode mode) noexcept
        -> void;
    auto release_cursor(this Window const& self) noexcept -> void;
    auto capture_cursor(this Window const& self) noexcept -> void;
    auto poll_events(this Window const& self) noexcept -> void;

    auto finish_frame(this Window& self) noexcept -> void;

public:
    static auto constexpr DEFAULT_WIDTH = u32{640};
    static auto constexpr DEFAULT_HEIGHT = u32{480};

private:
    GLFWwindow* glfw_window;
    std::unique_ptr<WindowData> data;
};

}  // namespace tmine
