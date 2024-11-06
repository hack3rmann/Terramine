#include "GUI.h"

#include "../window.hpp"
#include "../loaders.hpp"

using namespace tmine;

GUI::GUI()
: font{load_font("assets/fonts/font.fnt")}
, glyph_texture{Texture::from_image(
      load_png("assets/images/font.png"), TextureLoad::NO_MIPMAP_LINEAR
  )} {
    objectsButtons = 0;
    objectsSprites = 0;
    size = 0;
}

void GUI::addButton(
    float posX, float posY, float width, float height, Texture defTexture,
    Texture hoverTexture, Texture clickedTexture, std::string text,
    std::function<void()> function
) {
    buttons.emplace_back(
        this->font, this->glyph_texture, posX, posY, width,
        std::move(defTexture), std::move(hoverTexture),
        std::move(clickedTexture), text, function
    );

    objectsButtons++;
    size++;
}

void GUI::addSprite(
    float posX, float posY, float width, float height, Texture texture
) {
    sprites.emplace_back(posX, posY, width, std::move(texture));

    objectsSprites++;
    size++;
}

void GUI::render(glm::uvec2 viewport_size) {
    for (auto& sprite : this->sprites) {
        sprite.render(Window::aspect_ratio_of(viewport_size));
    }

    for (auto& button : this->buttons) {
        button.refreshState(viewport_size);
        button.render(viewport_size);
    }
}

int GUI::getObjects() { return size; }
