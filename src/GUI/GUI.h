#pragma once

#include <functional>

#include "Button.h"
#include "Sprite.h"

#define GUI_VERTEX_SIZE (2 + 2 + 4) /* XY TS RGBA */
#define GUI_VERTEX(buffer, X, Y, T, S, R, G, B, A) \
    buffer.emplace_back(glm::vec2{X, Y}, glm::vec2{T, S}, glm::vec4{R, G, B, A})
#define GUI_RECT(buffer, X, Y, W, H, R, G, B, A)                      \
    GUI_VERTEX(buffer, X - W / 2.0f, Y - H / 2, 0.0f, 0.0f, R, G, B, A); \
    GUI_VERTEX(buffer, X - W / 2.0f, Y + H / 2, 0.0f, 1.0f, R, G, B, A); \
    GUI_VERTEX(buffer, X + W / 2.0f, Y + H / 2, 1.0f, 1.0f, R, G, B, A); \
    GUI_VERTEX(buffer, X - W / 2.0f, Y - H / 2, 0.0f, 0.0f, R, G, B, A); \
    GUI_VERTEX(buffer, X + W / 2.0f, Y - H / 2, 1.0f, 0.0f, R, G, B, A); \
    GUI_VERTEX(buffer, X + W / 2.0f, Y + H / 2, 1.0f, 1.0f, R, G, B, A);

class GUI {
    friend class GUIHandler;

    int objectsButtons;
    int objectsSprites;
    int size;
    Button* buttons[64];
    Sprite* sprites[64];

public:
    GUI();
    void addButton(
        float posX, float posY, float width, float height,
        tmine::Texture defTexture, tmine::Texture hoverTexture,
        tmine::Texture clickTexture, std::string tex,
        std::function<void()> function
    );
    void addSprite(
        float posX, float posY, float width, float height,
        tmine::Texture texture
    );
    int getObjects();
    void render(glm::uvec2 window_size);
    void cleanUp();
};
