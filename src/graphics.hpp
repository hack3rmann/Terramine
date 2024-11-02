#pragma once

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

    auto bind(this Texture const& self, u32 slot) -> void;
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

    static auto from_source(ShaderSource const& source) -> ShaderProgram;

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

enum class Primitive {
    Points = GL_POINTS,
    LineStrip = GL_LINE_STRIP,
    LineLoop = GL_LINE_LOOP,
    Lines = GL_LINES,
    LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
    LinesAdjacency = GL_LINES_ADJACENCY,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
    Triangles = GL_TRIANGLES,
    TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
    TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
    Patches = GL_PATCHES,
};

class Mesh {
public:
    // TODO(hack3rmann): make span of attribute descriptors
    Mesh(
        std::vector<f32> vertices, std::span<usize const> attribute_sizes,
        Primitive primitive
    );
    ~Mesh();
    Mesh(Mesh const&) = delete;
    Mesh(Mesh&& other) noexcept;
    auto operator=(this Mesh&, Mesh const&) -> Mesh& = delete;
    auto operator=(this Mesh& self, Mesh&& other) noexcept -> Mesh&;

    template <typename Self>
    inline auto&& get_buffer(this Self&& self) noexcept {
        return std::forward<Self>(self).vertices;
    }

    auto reload_buffer(this Mesh const& self) noexcept -> void;
    auto draw(this Mesh const& self) -> void;

private:
    GLuint vertex_array_object_id{DUMMY_ID};
    GLuint vertex_buffer_object_id{DUMMY_ID};
    std::vector<f32> vertices;
    usize vertex_size;
    Primitive primitive;
    static auto constexpr DUMMY_ID = ~GLuint{0};
};

}  // namespace tmine
