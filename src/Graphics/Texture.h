#pragma once

#include "../Loaders/pngLoader.h"

class Texture {
public:
    GLuint id;
    int width, height;
    int slot;
    static int AviableSlot;
    Texture();
    Texture(GLuint id, int width, int height);
    ~Texture();

    void bind();
    void deleteTex();
    static void unbind();
};
