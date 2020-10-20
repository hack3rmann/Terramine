#pragma once
#ifndef PNG_LOADER_H
#define PNG_LOADER_H

#include <string>
#include <GL/glew.h>

class Texture;

extern Texture* load_texture(const char* file);
extern GLuint LoadGLTextures(const char* filename, int* width, int* height);
extern Texture* load_textureRGB(const char* file);
extern GLuint LoadGLTexturesRGB(const char* filename, int* width, int* height);
extern Texture* load_texture_NO_MIPMAP_LINEAR(const char* file);
extern GLuint LoadGLTextures_NO_MIPMAP_LINEAR(const char* filename, int* width, int* height);

#endif // !PNG_LOADER_H
