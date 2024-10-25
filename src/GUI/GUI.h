#pragma once

#include <functional>

#include "Button.h"
#include "Sprite.h"

#define GUI_VERTEX_SIZE (2 + 2 + 4) /* XY TS RGBA */
#define GUI_VERTEX(I, X, Y, T, S, R, G, B, A) \
    buffer[I + 0] = X;                        \
    buffer[I + 1] = Y;                        \
    buffer[I + 2] = T;                        \
    buffer[I + 3] = S;                        \
    buffer[I + 4] = R;                        \
    buffer[I + 5] = G;                        \
    buffer[I + 6] = B;                        \
    buffer[I + 7] = A;                        \
    I += GUI_VERTEX_SIZE;
#define GUI_RECT(I, X, Y, W, H, R, G, B, A)                      \
    GUI_VERTEX(I, X - W / 2, Y - H / 2, 0.0f, 0.0f, R, G, B, A); \
    GUI_VERTEX(I, X - W / 2, Y + H / 2, 0.0f, 1.0f, R, G, B, A); \
    GUI_VERTEX(I, X + W / 2, Y + H / 2, 1.0f, 1.0f, R, G, B, A); \
    GUI_VERTEX(I, X - W / 2, Y - H / 2, 0.0f, 0.0f, R, G, B, A); \
    GUI_VERTEX(I, X + W / 2, Y - H / 2, 1.0f, 0.0f, R, G, B, A); \
    GUI_VERTEX(I, X + W / 2, Y + H / 2, 1.0f, 1.0f, R, G, B, A);

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
    void render();
    void cleanUp();
};
