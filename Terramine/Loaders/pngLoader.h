#pragma once
#ifndef PNG_LOADER_H
#define PNG_LOADER_H

#include <string>
#include <GL/glew.h>

class Texture;

//extern int _png_load(const char* file, int* width, int* height);
extern Texture* load_texture(const char* file);
extern GLuint LoadGLTextures(const char* filename, int* width, int* height);
extern Texture* load_texture2(const char* file);
extern GLuint LoadGLTextures2(const char* filename, int* width, int* height);
//extern int LoadSOILTextures(const char* file, int* width, int* height);

#endif // !PNG_LOADER_H
