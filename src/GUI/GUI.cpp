#include "GUI.h"

#include "../window.hpp"
#include "../loaders.hpp"

using namespace tmine;

GUI::GUI()
: font{load_font("assets/fonts/font.fnt")}
, button_style{ButtonStyle{
      .textures =
          {
              Texture::from_image(
                  load_png("assets/images/testButtonDef.png"),
                  TextureLoad::DEFAULT
              ),
              Texture::from_image(
                  load_png("assets/images/testButtonHover.png"),
                  TextureLoad::DEFAULT
              ),
              Texture::from_image(
                  load_png("assets/images/testButtonClicked.png"),
                  TextureLoad::DEFAULT
              ),
          },
      .glyph_texture = Texture::from_image(
          load_png("assets/images/font.png"), TextureLoad::NO_MIPMAP_LINEAR
      ),
  }}
, shader{load_shader("gui_vertex.glsl", "gui_fragment.glsl")} {
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
        this->button_style,
        Text{
            this->font, this->button_style.glyph_texture, std::move(text),
            glm::vec2{posX, posY}, width
        },
        glm::vec2{posX, posY}, width
    );

    objectsButtons++;
    size++;
}

void GUI::addSprite(
    float posX, float posY, float width, float height, Texture texture
) {
    sprites.emplace_back(glm::vec2{posX, posY}, width, std::move(texture));

    objectsSprites++;
    size++;
}

void GUI::render(glm::uvec2 viewport_size) {
    for (auto& sprite : this->sprites) {
        sprite.render(this->shader, viewport_size);
    }

    for (auto& button : this->buttons) {
        button.render(this->shader, viewport_size);
    }
}

int GUI::getObjects() { return size; }
