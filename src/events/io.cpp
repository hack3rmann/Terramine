#include "../window.hpp"
#include "../events.hpp"

namespace tmine {

Input io{};

auto Input::update(this Input& self) noexcept -> void {
    self.current_frame += 1;
    self.mouse_delta = glm::vec2{0.0f};
}

auto Input::cursor_position_callback(GLFWwindow* window, f64 x, f64 y) -> void {
    auto data = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

    if (data->is_cursor_locked) {
        io.mouse_delta += glm::vec2{x, y} - io.mouse_pos;
    } else {
        io.is_cursor_started = true;
    }

    io.mouse_pos = glm::vec2{x, y};
}

auto Input::mouse_button_callback(
    [[maybe_unused]] GLFWwindow* window, i32 button, i32 action,
    [[maybe_unused]] i32 mode
) -> void {
    if (GLFW_PRESS == action) {
        io.key_states[Input::N_KEYBOARD_BUTTONS + button] = true;
        io.frames[Input::N_KEYBOARD_BUTTONS + button] = io.current_frame;
    } else if (GLFW_RELEASE == action) {
        io.key_states[Input::N_KEYBOARD_BUTTONS + button] = false;
        io.frames[Input::N_KEYBOARD_BUTTONS + button] = io.current_frame;
    }
}

void Input::key_callback(
    [[maybe_unused]] GLFWwindow* window, i32 key, [[maybe_unused]] i32 scancode,
    i32 action, [[maybe_unused]] i32 mode
) {
    if (GLFW_PRESS == action) {
        io.key_states[key] = true;
        io.frames[key] = io.current_frame;
    } else if (GLFW_RELEASE == action) {
        io.key_states[key] = false;
        io.frames[key] = io.current_frame;
    }
}

auto Input::is_pressed(this Input const& self, Key key) -> bool {
    return self.key_states[(usize) key];
}

auto Input::just_pressed(this Input& self, Key key) -> bool {
    return self.key_states[(usize) key] &&
           self.frames[(usize) key] == self.current_frame;
}

auto Input::is_clicked(this Input const& self, MouseButton button) -> bool {
    return self.key_states[Input::N_KEYBOARD_BUTTONS + (usize) button];
}

auto Input::just_clicked(this Input& self, MouseButton button) -> bool {
    return self.key_states[Input::N_KEYBOARD_BUTTONS + (usize) button] &&
           self.frames[Input::N_KEYBOARD_BUTTONS + (usize) button] ==
               self.current_frame;
}

auto Input::set_io_callbacks(GLFWwindow* window) -> void {
    glfwSetKeyCallback(window, Input::key_callback);
    glfwSetMouseButtonCallback(window, Input::mouse_button_callback);
    glfwSetCursorPosCallback(window, Input::cursor_position_callback);
}

}  // namespace tmine
