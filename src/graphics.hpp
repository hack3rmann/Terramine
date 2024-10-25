#pragma once

#include <glad/gl.h>

#include "types.hpp"
#include "data.hpp"

namespace tmine {

    using TextureLoadFlags = u32;

    struct TextureLoad {
        static constexpr TextureLoadFlags DEFAULT = 0;
        static constexpr TextureLoadFlags RGB = 1 << 0;
        static constexpr TextureLoadFlags NO_MIPMAP_LINEAR = 1 << 1;
    };

    class Texture {
    public:
        Texture(GLuint id, usize width, usize height);
        Texture();
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

}  // namespace tmine
