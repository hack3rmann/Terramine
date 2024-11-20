#include "../debug.hpp"
#include "../events.hpp"

namespace tmine {

namespace debug {

    auto update() -> void {
        if (io.just_pressed(Key::F3)) {
            DEBUG_IS_ENABLED = !DEBUG_IS_ENABLED;
        }
    }

}  // namespace debug

}  // namespace tmine
