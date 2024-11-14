#include "../graphics.hpp"
#include "../panic.hpp"

namespace tmine {

GeometryBufferData::GeometryBufferData(GeometryBufferData&& other) noexcept
: frame_buffer_object_id{other.frame_buffer_object_id}
, color_render_buffer_object_id{other.color_render_buffer_object_id}
, depth_render_buffer_object_id{other.depth_render_buffer_object_id}
, color_attachment_id{other.color_attachment_id}
, depth_attachment_id{other.depth_attachment_id} {
    other.frame_buffer_object_id = DUMMY_ID;
    other.color_render_buffer_object_id = DUMMY_ID;
    other.depth_render_buffer_object_id = DUMMY_ID;
    other.color_attachment_id = DUMMY_ID;
    other.depth_attachment_id = DUMMY_ID;
}

auto GeometryBufferData::operator=(
    this GeometryBufferData& self, GeometryBufferData&& other
) noexcept -> GeometryBufferData& {
    self.frame_buffer_object_id = other.frame_buffer_object_id;
    self.color_render_buffer_object_id = other.color_render_buffer_object_id;
    self.depth_render_buffer_object_id = other.depth_render_buffer_object_id;
    self.color_attachment_id = other.color_attachment_id;
    self.depth_attachment_id = other.depth_attachment_id;

    other.frame_buffer_object_id = DUMMY_ID;
    other.color_render_buffer_object_id = DUMMY_ID;
    other.depth_render_buffer_object_id = DUMMY_ID;
    other.color_attachment_id = DUMMY_ID;
    other.depth_attachment_id = DUMMY_ID;

    return self;
}

GeometryBufferData::~GeometryBufferData() {
    auto textures_to_delete = std::array{
        this->color_attachment_id,
        this->depth_attachment_id,
    };

    auto render_buffers_to_delete = std::array{
        this->color_render_buffer_object_id,
        this->depth_render_buffer_object_id,
    };

    glDeleteRenderbuffers(
        render_buffers_to_delete.size(), render_buffers_to_delete.data()
    );
    glDeleteTextures(textures_to_delete.size(), textures_to_delete.data());
    glDeleteFramebuffers(1, &this->frame_buffer_object_id);
}

GeometryBuffer::GeometryBuffer(glm::uvec2 viewport_size, u32 msaa_level)
: data{std::make_shared<GeometryBufferData>()}
, msaa_level{msaa_level}
, viewport_size{viewport_size} {
    glGenFramebuffers(1, &this->data->frame_buffer_object_id);
    glBindFramebuffer(GL_FRAMEBUFFER, this->data->frame_buffer_object_id);

    // Generate color texture
    glGenTextures(1, &this->data->color_attachment_id);
    glBindTexture(GL_TEXTURE_2D, this->data->color_attachment_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, viewport_size.x, viewport_size.y, 0, GL_RGB,
        GL_UNSIGNED_BYTE, 0
    );
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, viewport_size.x, viewport_size.y, 0, GL_RGB,
        GL_UNSIGNED_BYTE, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate depth texture
    glGenTextures(1, &this->data->depth_attachment_id);
    glBindTexture(GL_TEXTURE_2D, this->data->depth_attachment_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, viewport_size.x,
        viewport_size.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach frame buffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        this->data->color_attachment_id, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        this->data->depth_attachment_id, 0
    );

    // Generate color render texture
    glGenRenderbuffers(1, &this->data->color_render_buffer_object_id);
    glBindRenderbuffer(
        GL_RENDERBUFFER, this->data->color_render_buffer_object_id
    );
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, msaa_level, GL_RGB8, viewport_size.x, viewport_size.y
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
        this->data->color_render_buffer_object_id
    );

    // Generate depth render texture
    glGenRenderbuffers(1, &this->data->depth_render_buffer_object_id);
    glBindRenderbuffer(
        GL_RENDERBUFFER, this->data->depth_render_buffer_object_id
    );
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, msaa_level, GL_DEPTH_COMPONENT, viewport_size.x,
        viewport_size.y
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
        this->data->depth_render_buffer_object_id
    );

    auto const status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE: {
    } break;
    case GL_FRAMEBUFFER_UNSUPPORTED: {
        throw Panic("Frame buffer is unsupported...");
    } break;
    default: {
        throw Panic("Frame buffer is not complete...");
    } break;
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto GeometryBuffer::bind_frame_buffer(this GeometryBuffer const& self)
    -> void {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, self.data->frame_buffer_object_id);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        self.data->color_attachment_id, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        self.data->depth_attachment_id, 0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw Panic("failed to bind incomplete frame buffer");
    }
}

auto GeometryBuffer::unbind_all() -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace tmine
