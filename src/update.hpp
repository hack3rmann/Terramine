#pragma once

#include <concepts>
#include <chrono>

#include "types.hpp"

namespace tmine {

class FixedUpdater {
public:
    explicit FixedUpdater(f32 time_step = DEFAULT_TIME_STEP);

    auto fixed_update(this FixedUpdater& self, std::invocable<f32> auto update)
        -> void {
        auto duration = self.previous_frame_reminder + self.frame_duration;

        while (self.time_step < duration) {
            duration -= self.time_step;
            update(self.time_step);
        }

        self.current_frame_reminder = duration;
    }

    inline auto get_time_step(this FixedUpdater const& self) noexcept -> f32 {
        return self.time_step;
    }

    auto start_new_frame(this FixedUpdater& self) -> void;

public:
    static auto constexpr DEFAULT_UPDATE_COUNT = 20.0f;
    static auto constexpr DEFAULT_TIME_STEP = 1.0f / 60.0f / DEFAULT_UPDATE_COUNT;

private:
    f32 time_step{DEFAULT_TIME_STEP};
    f32 previous_frame_reminder{0.0f};
    f32 current_frame_reminder{0.0f};
    f32 frame_duration{DEFAULT_UPDATE_COUNT * DEFAULT_TIME_STEP};
    std::chrono::time_point<std::chrono::high_resolution_clock> prev_time;
};

}  // namespace tmine
