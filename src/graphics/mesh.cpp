#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <ranges>

#include "../graphics.hpp"

namespace rg = std::ranges;
namespace vs = rg::views;

namespace tmine {

Mesh::Mesh(
    std::vector<f32> vertices, std::span<usize const> attribute_sizes,
    Primitive primitive
)
: vertices{std::move(vertices)}
, vertex_size{rg::fold_left(attribute_sizes, usize{0}, std::plus{})}
, primitive{primitive} {
    glGenVertexArrays(1, &this->vertex_array_object_id);
    glGenBuffers(1, &this->vertex_buffer_object_id);

    glBindVertexArray(vertex_array_object_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_id);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(this->vertices[0]) * this->vertices.size(),
        this->vertices.data(), GL_STATIC_DRAW
    );

    auto offset = usize{0};

    for (auto const [i, size] : attribute_sizes | vs::enumerate) {
        glVertexAttribPointer(
            i, size, GL_FLOAT, GL_FALSE,
            this->vertex_size * sizeof(this->vertices[0]),
            (GLvoid*) (offset * sizeof(this->vertices[0]))
        );

        glEnableVertexAttribArray(i);

        offset += size;
    }

    glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other) noexcept
: vertex_array_object_id{other.vertex_array_object_id}
, vertex_buffer_object_id{other.vertex_buffer_object_id}
, vertices{std::move(other.vertices)}
, vertex_size{other.vertex_size}
, primitive{other.primitive} {
    other.vertex_array_object_id = Mesh::DUMMY_ID;
    other.vertex_buffer_object_id = Mesh::DUMMY_ID;
    other.vertex_size = 0;
}

auto Mesh::operator=(this Mesh& self, Mesh&& other) noexcept -> Mesh& {
    self.vertex_array_object_id = other.vertex_array_object_id;
    self.vertex_buffer_object_id = other.vertex_buffer_object_id;
    self.vertices = std::move(other.vertices);
    self.vertex_size = other.vertex_size;
    self.primitive = other.primitive;

    other.vertex_array_object_id = Mesh::DUMMY_ID;
    other.vertex_buffer_object_id = Mesh::DUMMY_ID;
    other.vertex_size = 0;
    other.primitive = Primitive::Points;

    return self;
}

auto Mesh::draw(this Mesh const& self) -> void {
    if (!self.vertices.empty()) {
        glBindVertexArray(self.vertex_array_object_id);
        glDrawArrays((GLuint) self.primitive, 0, self.vertices.size());
        glBindVertexArray(0);
    }
}

Mesh::~Mesh() {
    if (this->vertex_buffer_object_id == Mesh::DUMMY_ID ||
        this->vertex_array_object_id == Mesh::DUMMY_ID)
    {
        return;
    }

    glDeleteVertexArrays(1, &vertex_array_object_id);
    glDeleteBuffers(1, &vertex_buffer_object_id);
}

void Mesh::reload_buffer(this Mesh const& self) noexcept {
    glBindVertexArray(self.vertex_array_object_id);
    glBindBuffer(GL_ARRAY_BUFFER, self.vertex_buffer_object_id);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(self.vertices[0]) * self.vertices.size(),
        self.vertices.data(), GL_STATIC_DRAW
    );
}

}  // namespace tmine
