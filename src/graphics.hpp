#pragma once

#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <ranges>

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

struct TextureData {
    GLuint id{DUMMY_ID};
    glm::uvec2 size{0, 0};
    static auto constexpr DUMMY_ID = GLuint{0};

    ~TextureData();

    inline explicit TextureData(GLuint id, glm::uvec2 size)
    : id{id}
    , size{size} {}

    TextureData(TextureData&) = delete;

    inline TextureData(TextureData&& other) noexcept
    : id{other.id} {
        other.id = DUMMY_ID;
    }

    auto operator=(this TextureData&, TextureData&) -> TextureData& = delete;

    inline auto operator=(this TextureData& self, TextureData&& other) noexcept
        -> TextureData& {
        self.id = other.id;
        other.id = DUMMY_ID;
        return self;
    }
};

class Texture {
public:
    Texture(GLuint id, glm::uvec2 size);
    Texture() = default;

    static auto from_image(
        Image const& image, TextureLoadFlags flags = TextureLoad::DEFAULT
    ) noexcept -> Texture;

    auto bind(this Texture const& self, u32 slot) -> void;
    static auto unbind(u32 slot) -> void;

    auto get_size(this Texture const& self) -> glm::uvec2 {
        return self.data->size;
    }

    auto get_aspect_ratio(this Texture const& self) -> f32 {
        return (f32) self.data->size.x / (f32) self.data->size.y;
    }

private:
    std::shared_ptr<TextureData> data;
};

struct ShaderData {
    GLuint id{DUMMY_ID};
    static auto constexpr DUMMY_ID = GLuint{0};

    ~ShaderData();

    inline explicit ShaderData(GLuint id)
    : id{id} {}

    ShaderData(ShaderData&) = delete;

    inline ShaderData(ShaderData&& other) noexcept
    : id{other.id} {
        other.id = DUMMY_ID;
    }

    auto operator=(this ShaderData&, ShaderData&) -> ShaderData& = delete;

    auto operator=(this ShaderData& self, ShaderData&& other) noexcept
        -> ShaderData& {
        self.id = other.id;
        other.id = DUMMY_ID;
        return self;
    }
};

class ShaderProgram {
public:
    ShaderProgram(GLuint id);
    ShaderProgram() = default;

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
    std::shared_ptr<ShaderData> data;
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

template <class T>
concept WithAttributes = requires(T value) {
    {
        T::ATTRIBUTE_SIZES
    } -> std::convertible_to<std::span<usize const>>;
};

template <class T, class V>
concept MeshBuffer = requires(T buffer, V vertex) {
    requires std::is_move_constructible_v<T>;
    requires std::is_move_assignable_v<T>;
    requires std::is_default_constructible_v<T>;

    {
        buffer.data()
    } -> std::same_as<V*>;
    {
        buffer.size()
    } -> std::convertible_to<usize>;
    {
        buffer.empty()
    } -> std::same_as<bool>;
    buffer.clear();
    {
        buffer[0]
    } -> std::convertible_to<V&>;
};

template <WithAttributes V, MeshBuffer<V> Buffer = std::vector<V>>
class BufferedMesh {
public:
    // TODO(hack3rmann): make span of attribute descriptors
    BufferedMesh(Buffer vertices, Primitive primitive)
    : vertices{std::move(vertices)}
    , primitive{primitive} {
        namespace vs = std::ranges::views;

        glGenVertexArrays(1, &this->vertex_array_object_id);
        glGenBuffers(1, &this->vertex_buffer_object_id);

        glBindVertexArray(vertex_array_object_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_id);
        glBufferData(
            GL_ARRAY_BUFFER, sizeof(this->vertices[0]) * this->vertices.size(),
            this->vertices.data(), GL_STATIC_DRAW
        );

        auto offset = usize{0};

        for (auto const [i, size] : V::ATTRIBUTE_SIZES | vs::enumerate) {
            glVertexAttribPointer(
                i, size, GL_FLOAT, GL_FALSE, sizeof(V),
                (GLvoid*) (offset * sizeof(f32))
            );

            glEnableVertexAttribArray(i);

            offset += size;
        }

        glBindVertexArray(0);
    }

    BufferedMesh(Primitive primitive)
    : BufferedMesh{{}, primitive} {}

    BufferedMesh()
    : BufferedMesh{Primitive::Triangles} {}

    ~BufferedMesh() {
        if (this->vertex_buffer_object_id == BufferedMesh::DUMMY_ID ||
            this->vertex_array_object_id == BufferedMesh::DUMMY_ID)
        {
            return;
        }

        glDeleteBuffers(1, &vertex_buffer_object_id);
        glDeleteVertexArrays(1, &vertex_array_object_id);
    }

    BufferedMesh(BufferedMesh const& other)
    : BufferedMesh(other.vertices, other.primitive) {}

    BufferedMesh(BufferedMesh&& other) noexcept
    : vertex_array_object_id{other.vertex_array_object_id}
    , vertex_buffer_object_id{other.vertex_buffer_object_id}
    , vertices{std::move(other.vertices)}
    , primitive{other.primitive} {
        other.vertex_array_object_id = BufferedMesh::DUMMY_ID;
        other.vertex_buffer_object_id = BufferedMesh::DUMMY_ID;
    }

    auto operator=(this BufferedMesh& self, BufferedMesh const& other)
        -> BufferedMesh& {
        auto clone = other;
        self = std::move(clone);
        return self;
    }

    auto operator=(this BufferedMesh& self, BufferedMesh&& other) noexcept
        -> BufferedMesh& {
        self.vertex_array_object_id = other.vertex_array_object_id;
        self.vertex_buffer_object_id = other.vertex_buffer_object_id;
        self.vertices = std::move(other.vertices);
        self.primitive = other.primitive;

        other.vertex_array_object_id = BufferedMesh::DUMMY_ID;
        other.vertex_buffer_object_id = BufferedMesh::DUMMY_ID;
        other.primitive = Primitive::Points;

        return self;
    }

    template <typename Self>
    inline auto&& get_buffer(this Self&& self) noexcept {
        return std::forward<Self>(self).vertices;
    }

    auto reload_buffer(this BufferedMesh const& self) noexcept -> void {
        glBindVertexArray(self.vertex_array_object_id);
        glBindBuffer(GL_ARRAY_BUFFER, self.vertex_buffer_object_id);
        glBufferData(
            GL_ARRAY_BUFFER, sizeof(self.vertices[0]) * self.vertices.size(),
            (void*) self.vertices.data(), GL_STATIC_DRAW
        );
    }

    auto draw(this BufferedMesh const& self) -> void {
        if (self.vertices.empty()) {
            return;
        }

        glBindVertexArray(self.vertex_array_object_id);
        glDrawArrays(
            (GLuint) self.primitive, 0, sizeof(V) * self.vertices.size()
        );
    }

private:
    GLuint vertex_array_object_id{DUMMY_ID};
    GLuint vertex_buffer_object_id{DUMMY_ID};
    Buffer vertices{};
    Primitive primitive{Primitive::Points};
    static auto constexpr DUMMY_ID = ~GLuint{0};
};

template <WithAttributes V>
using Mesh = BufferedMesh<V, std::vector<V>>;

}  // namespace tmine
