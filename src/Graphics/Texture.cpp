#include "Texture.h"

#include <glad/gl.h>

#include <iostream>
#include <string>

#include "../defines.cpp"

int Texture::AviableSlot = 0;

Texture::Texture(GLuint id, int width, int height)
    : id(id)
    , width(width)
    , height(height) {
    slot = AviableSlot + GL_TEXTURE0;
    AviableSlot++;
}

Texture::Texture() {}

Texture::~Texture() {}

void Texture::bind() { glcall(glBindTexture(GL_TEXTURE_2D, id)); }

void Texture::unbind() { glcall(glBindTexture(GL_TEXTURE_2D, 0)); }

void Texture::deleteTex() { glcall(glDeleteTextures(1, &id)); }
