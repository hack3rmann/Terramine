#include "window.hpp"
#include "game.hpp"

using namespace tmine;

auto main() -> int {
    auto window = Window{"Terramine"};
    auto game = Game{window.size()};

    while (window.is_open()) {
        game.render(window.size());
        game.update(&window);

        window.finish_frame();
    }
}
