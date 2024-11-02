#pragma once

#include <string>
#include <glad/gl.h>

#include "../graphics.hpp"

class FrameBuffer {
    friend class SceneHandler;
    std::string vName, fName;

    tmine::ShaderProgram screenShader;
    static float screenQuad[24];
    GLuint vao;
    GLuint vbo;

    GLuint fbo;
    GLuint color_rbo;
    GLuint depth_rbo;
    GLuint colorAtt;
    GLuint depthAtt;
    int msaa;

    int width, height;

public:
    FrameBuffer(
        std::string const& vName, std::string const& fName,
        glm::uvec2 window_size
    );
    FrameBuffer(glm::uvec2 window_size);
    void terminate();
    void bind();
    void drawColor();
    void drawDepth();
    void drawBoth();
    void bindColorTex();
    void bindDepthTex();
    void reload(glm::uvec2 window_size);
    void reload(
        std::string const& vName, std::string const& fName,
        glm::uvec2 window_size
    );
    void refreshShader();
    int check();
    static void unbind();
};
