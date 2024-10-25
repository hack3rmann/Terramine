#pragma once

#include <glad/gl.h>

#include <string>

class Texture;

extern Texture* load_texture(char const* file);
extern GLuint LoadGLTextures(char const* filename, int* width, int* height);
extern Texture* load_textureRGB(char const* file);
extern GLuint LoadGLTexturesRGB(char const* filename, int* width, int* height);
extern Texture* load_texture_NO_MIPMAP_LINEAR(char const* file);
extern GLuint LoadGLTextures_NO_MIPMAP_LINEAR(
    char const* filename, int* width, int* height
);
