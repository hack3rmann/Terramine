#include <chrono>

#include "../debug.hpp"
#include "../update.hpp"

namespace tmine {

FixedUpdater::FixedUpdater(f32 time_step)
: time_step{time_step}
, prev_time{std::chrono::high_resolution_clock::now()} {}

auto FixedUpdater::start_new_frame(this FixedUpdater& self) -> void {
    self.previous_frame_reminder = self.current_frame_reminder;

    auto const now = std::chrono::high_resolution_clock::now();
    self.frame_duration =
        std::chrono::duration<f32>{now - self.prev_time}.count();
    self.prev_time = now;

    debug::text()->set(
        "fps", fmt::format("FPS: {:.1f}", 1.0f / self.frame_duration)
    );
}
}  // namespace tmine
