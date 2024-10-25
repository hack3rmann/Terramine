#include <glad/gl.h>
#include <cstdio>

#include "../graphics.hpp"

using namespace tmine;

Texture::Texture(GLuint id, usize width, usize height)
: id{id}
, width{width}
, height{height}
, slot{AVAILABLE_SLOT + GL_TEXTURE0}
, n_clones_ptr{new usize{1}} {
    AVAILABLE_SLOT += 1;
}

Texture::~Texture() {
    if (Texture::DUMMY_ID == this->id) {
        return;
    } else if (*this->n_clones_ptr > 1) {
        *this->n_clones_ptr -= 1;
        return;
    }

    delete this->n_clones_ptr;
    glDeleteTextures(1, &this->id);
}

Texture::Texture(Texture const& other)
: id{other.id}
, width{other.width}
, height{other.height}
, slot{other.slot}
, n_clones_ptr{other.n_clones_ptr} {
    *other.n_clones_ptr += 1;
}

Texture::Texture(Texture&& other) noexcept
: id{other.id}
, width{other.width}
, height{other.height}
, slot{other.slot}
, n_clones_ptr{other.n_clones_ptr} {
    other.id = Texture::DUMMY_ID;
    other.width = 0;
    other.height = 0;
    other.slot = 0;
    other.n_clones_ptr = nullptr;
}

auto Texture::operator=(this Texture& self, Texture const& other) -> Texture& {
    self.id = other.id;
    self.width = other.width;
    self.height = other.height;
    self.slot = other.slot;
    self.n_clones_ptr = other.n_clones_ptr;

    *self.n_clones_ptr += 1;

    return self;
}

auto Texture::operator=(this Texture& self, Texture&& other) noexcept
    -> Texture& {
    self.id = other.id;
    self.width = other.width;
    self.height = other.height;
    self.slot = other.slot;
    self.n_clones_ptr = other.n_clones_ptr;

    other.id = Texture::DUMMY_ID;
    other.width = 0;
    other.height = 0;
    other.slot = 0;
    other.n_clones_ptr = nullptr;

    return self;
}

void Texture::bind(this Texture const& self) {
    glBindTexture(GL_TEXTURE_2D, self.id);
}

auto Texture::unbind([[maybe_unused]] this Texture const& self) -> void {
    Texture::unbind_all();
}

auto Texture::unbind_all() -> void { glBindTexture(GL_TEXTURE_2D, 0); }

auto Texture::from_image(Image const& image, TextureLoadFlags flags) noexcept
    -> Texture {
    GLuint id = 0;

    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto const format = flags & TextureLoad::RGB ? GL_RGB : GL_RGBA;

    glTexImage2D(
        GL_TEXTURE_2D, 0, format, image.get_width(), image.get_height(), 0,
        format, GL_UNSIGNED_BYTE, image.get_data().data()
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

    return Texture{id, image.get_width(), image.get_height()};
}
