#include <fmt/format.h>

#include "../window.hpp"
#include "../events.hpp"
#include "../panic.hpp"

namespace tmine {

static void window_size_callback(GLFWwindow* window, i32 width, i32 height) {
    glViewport(0, 0, width, height);

    auto data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
    data->size.x = width;
    data->size.y = height;
}

[[noreturn]] static auto error_callback(int id, char const* description)
    -> void {
    throw Panic("GLFW error {}: {}", id, description);
}

Window::Window(char const* title, u32 width, u32 height)
: glfw_window{nullptr}
, data{std::make_unique<WindowData>(glm::uvec2{width, height}, false)} {
    if (!glfwInit()) {
        throw Panic("failed to initialize GLFW");
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    this->glfw_window =
        glfwCreateWindow(width, height, title, nullptr, nullptr);

    glfwSetWindowUserPointer(this->glfw_window, this->data.get());

    if (nullptr == this->glfw_window) {
        glfwTerminate();

        throw Panic("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(this->glfw_window);

    gladLoadGL(glfwGetProcAddress);

    glViewport(0, 0, width, height);

    glfwSetWindowSizeCallback(glfw_window, window_size_callback);

    Input::set_io_callbacks(glfw_window);
}

Window::~Window() {
    glfwDestroyWindow(this->glfw_window);
    glfwTerminate();
}

Window::Window(Window&& other)
: glfw_window{other.glfw_window}
, data{std::move(other.data)} {
    other.glfw_window = nullptr;
}

auto Window::operator=(this Window& self, Window&& other) -> Window& {
    self.glfw_window = other.glfw_window;
    self.data = std::move(other.data);

    other.glfw_window = nullptr;

    return self;
}

auto Window::aspect_ratio_of(glm::uvec2 size) noexcept -> f32 {
    return (f32) size.x / (f32) size.y;
}

auto Window::is_open(this Window const& self) noexcept -> bool {
    return !glfwWindowShouldClose(self.glfw_window);
}

auto Window::schedule_close(this Window const& self) noexcept -> void {
    glfwSetWindowShouldClose(self.glfw_window, true);
}

auto Window::swap_buffers(this Window const& self) noexcept -> void {
    glfwSwapBuffers(self.glfw_window);
}

auto Window::set_cursor_mode(this Window const& self, CursorMode mode) noexcept
    -> void {
    glfwSetInputMode(self.glfw_window, GLFW_CURSOR, (u32) mode);
}

auto Window::release_cursor(this Window const& self) noexcept -> void {
    self.data->is_cursor_locked = false;
    self.set_cursor_mode(CursorMode::Normal);
}

auto Window::capture_cursor(this Window const& self) noexcept -> void {
    self.data->is_cursor_locked = true;
    self.set_cursor_mode(CursorMode::Disabled);
}

auto Window::poll_events([[maybe_unused]] this Window const& self) noexcept
    -> void {
    glfwPollEvents();
}

auto Window::toggle_cursor_visibility(this Window& self) -> void {
    self.data->is_cursor_locked = !self.data->is_cursor_locked;

    self.set_cursor_mode(
        self.data->is_cursor_locked ? CursorMode::Disabled : CursorMode::Normal
    );
}

auto Window::finish_frame(this Window& self) noexcept -> void {
    io.update();
    self.swap_buffers();
    self.poll_events();
}

}  // namespace tmine
