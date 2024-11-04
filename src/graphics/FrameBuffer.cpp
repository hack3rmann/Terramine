#include "FrameBuffer.h"
#include "../loaders.hpp"

using namespace tmine;

float FrameBuffer::screenQuad[24] = {
    //    X      Y       T     S
    -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

FrameBufferData::~FrameBufferData() {
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
    glDeleteBuffers(1, &this->vertex_buffer_object_id);
    glDeleteVertexArrays(1, &this->vertex_array_object_id);
    glDeleteTextures(textures_to_delete.size(), textures_to_delete.data());
    glDeleteFramebuffers(1, &this->frame_buffer_object_id);
}

FrameBuffer::FrameBuffer(
    std::string const& vName, std::string const& fName, glm::uvec2 window_size
)
: vName{std::move(vName)}
, fName{std::move(fName)}
, data{std::make_shared<FrameBufferData>()}
, width{(int) window_size.x}
, height{(int) window_size.y} {
    msaa = 4;
    glGenFramebuffers(1, &this->data->frame_buffer_object_id);
    glBindFramebuffer(GL_FRAMEBUFFER, this->data->frame_buffer_object_id);

    width = window_size.x;
    height = window_size.y;

    /* Color attachment */
    glGenTextures(1, &this->data->color_attachment_id);
    glBindTexture(GL_TEXTURE_2D, this->data->color_attachment_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0
    );
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Depth color attachment */
    glGenTextures(1, &this->data->depth_attachment_id);
    glBindTexture(GL_TEXTURE_2D, this->data->depth_attachment_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Attaching */
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        this->data->color_attachment_id, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        this->data->depth_attachment_id, 0
    );

    /* Screen render vertices */
    glGenVertexArrays(1, &this->data->vertex_array_object_id);
    glGenBuffers(1, &this->data->vertex_buffer_object_id);

    glBindVertexArray(this->data->vertex_array_object_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->data->vertex_buffer_object_id);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, screenQuad, GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (GLvoid*) (0 * sizeof(float))
    );
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    screenShader = load_shader(vName.c_str(), fName.c_str());

    /* Attaching color renderbuffer */
    glGenRenderbuffers(1, &this->data->color_render_buffer_object_id);
    glBindRenderbuffer(
        GL_RENDERBUFFER, this->data->color_render_buffer_object_id
    );
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, msaa, GL_RGB8, width, height
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
        this->data->color_render_buffer_object_id
    );

    /* Attaching depth renderbuffer */
    glGenRenderbuffers(1, &this->data->depth_render_buffer_object_id);
    glBindRenderbuffer(
        GL_RENDERBUFFER, this->data->depth_render_buffer_object_id
    );
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, msaa, GL_DEPTH_COMPONENT, width, height
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
        this->data->depth_render_buffer_object_id
    );

    /* Check OK */
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        fprintf(stderr, "Frame buffer is unsupported...");
        break;
    default:
        fprintf(stderr, "Frame buffer is not complete...");
        break;
    }

    /* Unbinding */
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bind() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, this->data->frame_buffer_object_id);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        this->data->color_attachment_id, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        this->data->depth_attachment_id, 0
    );
}

void FrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void FrameBuffer::drawColor() {
    screenShader.bind();

    /* Vertices */
    glBindVertexArray(this->data->vertex_array_object_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->data->vertex_buffer_object_id);

    /* Binding texture */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->data->color_attachment_id);

    screenShader.uniform_int("screenColor", 0);
    screenShader.uniform_vec2(
        "resolution", glm::vec2{this->width, this->height}
    );

    /* Draw */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Unbind vertext array */
    glBindVertexArray(0);
}

void FrameBuffer::drawDepth() {
    screenShader.bind();

    /* Vertices */
    glBindVertexArray(this->data->vertex_array_object_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->data->vertex_buffer_object_id);

    /* Binding texture */
    glBindTexture(GL_TEXTURE_2D, this->data->depth_attachment_id);

    /* Draw */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Unbind vertext array */
    glBindVertexArray(0);
}

void FrameBuffer::drawBoth() {
    screenShader.bind();

    /* Vertices */
    glBindVertexArray(this->data->vertex_array_object_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->data->vertex_buffer_object_id);

    /* Shader uniforms */
    screenShader.uniform_int("screenDepth", 0);
    screenShader.uniform_int("screenColor", 1);
    screenShader.uniform_vec2(
        "resolution", glm::vec2{this->width, this->height}
    );

    /* Binding Textures */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->data->depth_attachment_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->data->color_attachment_id);

    /* Draw */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Unbind array */
    glBindVertexArray(0);
}

int FrameBuffer::check() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBuffer::refreshShader() {
    screenShader = ShaderProgram::from_source(
        load_shader_source(vName.c_str(), fName.c_str())
    );
}

void FrameBuffer::bindColorTex() {
    glBindTexture(GL_TEXTURE_2D, this->data->color_attachment_id);
}

void FrameBuffer::bindDepthTex() {
    glBindTexture(GL_TEXTURE_2D, this->data->depth_attachment_id);
}
