#pragma once

#include <optional>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "types.hpp"
#include "data.hpp"

namespace tmine {

using TextureLoadFlags = u32;

struct TextureLoad {
    static TextureLoadFlags constexpr DEFAULT = 0;
    static TextureLoadFlags constexpr RGB = 1 << 0;
    static TextureLoadFlags constexpr NO_MIPMAP_LINEAR = 1 << 1;
};

class Texture {
public:
    Texture(GLuint id, usize width, usize height);
    Texture() = default;
    ~Texture();
    Texture(Texture const&);
    Texture(Texture&&) noexcept;
    auto operator=(this Texture& self, Texture const&) -> Texture&;
    auto operator=(this Texture& self, Texture&&) noexcept -> Texture&;

    static auto from_image(Image const& image, TextureLoadFlags flags) noexcept
        -> Texture;

    auto bind(this Texture const& self) -> void;
    auto unbind(this Texture const& self) -> void;

    static auto unbind_all() -> void;

private:
    GLuint id{DUMMY_ID};
    usize width{0};
    usize height{0};
    usize slot{0};
    volatile usize* n_clones_ptr{nullptr};
    inline static auto AVAILABLE_SLOT = usize{0};
    static auto constexpr DUMMY_ID = ~GLuint{0};
};

class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();
    ShaderProgram(GLuint id);
    ShaderProgram(ShaderProgram const&);
    ShaderProgram(ShaderProgram&&) noexcept;

    auto operator=(this ShaderProgram& self, ShaderProgram const&)
        -> ShaderProgram&;
    auto operator=(this ShaderProgram& self, ShaderProgram&&) noexcept
        -> ShaderProgram&;

    static auto from_source(ShaderSource const& source
    ) -> std::optional<ShaderProgram>;

    auto bind(this ShaderProgram const& self) -> void;

    auto uniform_mat4(
        this ShaderProgram const& self, char const* name, glm::mat4 matrix
    ) -> void;
    auto uniform_vec2(
        this ShaderProgram const& self, char const* name, glm::vec2 vec
    ) -> void;
    auto uniform_vec3(
        this ShaderProgram const& self, char const* name, glm::vec3 vec
    ) -> void;
    auto uniform_int(this ShaderProgram const& self, char const* name, i32 num)
        -> void;

private:
    GLuint id{DUMMY_ID};
    volatile usize* n_clones_ptr{nullptr};
    static auto constexpr DUMMY_ID = ~GLuint{0};
};

}  // namespace tmine
