#include <glad/gl.h>
#include <cstdio>
#include <fmt/printf.h>

#include "../graphics.hpp"

using namespace tmine;

TextureData::~TextureData() { glDeleteTextures(1, &this->id); }

Texture::Texture(GLuint id, glm::uvec2 size)
: data{std::make_shared<TextureData>(id, size)} {}

void Texture::bind(this Texture const& self, u32 slot) {
    if (nullptr == self.data) {
        return;
    }

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, self.data->id);
}

auto Texture::unbind(u32 slot) -> void { glBindTexture(GL_TEXTURE_2D, slot); }

auto Texture::from_image(Image const& image, TextureLoadFlags flags) noexcept
    -> Texture {
    GLuint id = 0;

    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto const format = flags & TextureLoad::RGB ? GL_RGB : GL_RGBA;

    glTexImage2D(
        GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format,
        GL_UNSIGNED_BYTE, image.data.data()
    );

    if (flags & TextureLoad::NO_MIPMAP_LINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture{id, glm::uvec2{image.width, image.height}};
}
