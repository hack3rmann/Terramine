#pragma once

#include <string>

#include "GL/glew.h"
#include "Shader.h"

class FrameBuffer {
    friend class SceneHandler;
    std::string vName, fName;

    Shader* screenShader;
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
    FrameBuffer(std::string const& vName, std::string const& fName);
    FrameBuffer();
    void terminate();
    void bind();
    void drawColor();
    void drawDepth();
    void drawBoth();
    void bindColorTex();
    void bindDepthTex();
    void reload();
    void reload(std::string const& vName, std::string const& fName);
    void refreshShader();
    int check();
    static void unbind();
};
