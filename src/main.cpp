#include "window.hpp"
#include "game.hpp"

using namespace tmine;

auto main() -> int {
    auto window = Window{"Terramine"};
    auto game = Game{window.get_size()};

    while (window.is_open()) {
        game.render(window.get_size());
        game.update(&window);

        window.finish_frame();
    }
}
