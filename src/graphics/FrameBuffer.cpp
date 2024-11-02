#include "FrameBuffer.h"

#include "../Window.h"
#include "../loaders.hpp"

using namespace tmine;

float FrameBuffer::screenQuad[24] = {
    //    X      Y       T     S
    /*-1.0f,  0.5f,   0.0f, 0.0f,
    -1.0f,  1.0f,   0.0f, 1.0f,
    -0.5f,  1.0f,   1.0f, 1.0f,
    -1.0f,  0.5f,   0.0f, 0.0f,
    -0.5f,  0.5f,   1.0f, 0.0f,
    -0.5f,  1.0f,   1.0f, 1.0f*/
    -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

FrameBuffer::FrameBuffer(std::string const& vName, std::string const& fName) {
    reload(vName, fName);
}

FrameBuffer::FrameBuffer() { reload(); }

void FrameBuffer::bind() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAtt, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAtt, 0
    );
}

void FrameBuffer::reload(std::string const& vName, std::string const& fName) {
    this->vName = vName;
    this->fName = fName;
    msaa = 4;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    width = Window::width;
    height = Window::height;

    /* Color attachment */
    glGenTextures(1, &colorAtt);
    glBindTexture(GL_TEXTURE_2D, colorAtt);
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
    glGenTextures(1, &depthAtt);
    glBindTexture(GL_TEXTURE_2D, depthAtt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Attaching */
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAtt, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAtt, 0
    );

    /* Screen render vertices */
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
    glGenRenderbuffers(1, &color_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, msaa, GL_RGB8, width, height
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rbo
    );

    /* Attaching depth renderbuffer */
    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, msaa, GL_DEPTH_COMPONENT, width, height
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo
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

void FrameBuffer::reload() {
    msaa = 4;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    width = Window::width;
    height = Window::height;

    /* Color attachment */
    glGenTextures(1, &colorAtt);
    glBindTexture(GL_TEXTURE_2D, colorAtt);
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
    glGenTextures(1, &depthAtt);
    glBindTexture(GL_TEXTURE_2D, depthAtt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Attaching */
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAtt, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAtt, 0
    );

    /* Screen render vertices */
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, screenQuad, GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (GLvoid*) (0 * sizeof(float))
    );
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    /* Attaching color renderbuffer */
    glGenRenderbuffers(1, &color_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, msaa, GL_RGB8, width, height
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rbo
    );

    /* Attaching depth renderbuffer */
    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, msaa, GL_DEPTH_COMPONENT, width, height
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo
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

void FrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void FrameBuffer::terminate() { glDeleteFramebuffers(1, &fbo); }

void FrameBuffer::drawColor() {
    screenShader.bind();

    /* Vertices */
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    /* Binding texture */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorAtt);

    /* Draw */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Unbind vertext array */
    glBindVertexArray(0);
}

void FrameBuffer::drawDepth() {
    screenShader.bind();

    /* Vertices */
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    /* Binding texture */
    glBindTexture(GL_TEXTURE_2D, depthAtt);

    /* Draw */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Unbind vertext array */
    glBindVertexArray(0);
}

void FrameBuffer::drawBoth() {
    screenShader.bind();

    /* Vertices */
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    /* Shader uniforms */
    screenShader.uniform_int("screenDepth", 0);
    screenShader.uniform_int("screenColor", 1);

    /* Binding Textures */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthAtt);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colorAtt);

    /* Draw */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Unbind array */
    glBindVertexArray(0);
}

int FrameBuffer::check() {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        return 1;
    }
    return 0;
}

void FrameBuffer::refreshShader() {
    screenShader = ShaderProgram::from_source(
        load_shader_source(vName.c_str(), fName.c_str())
    );
}

void FrameBuffer::bindColorTex() { glBindTexture(GL_TEXTURE_2D, colorAtt); }

void FrameBuffer::bindDepthTex() { glBindTexture(GL_TEXTURE_2D, depthAtt); }
