#include "Texture.h"

#include <glad/gl.h>

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

void Texture::bind() { glBindTexture(GL_TEXTURE_2D, id); }

void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::deleteTex() { glDeleteTextures(1, &id); }
