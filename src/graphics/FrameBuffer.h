#pragma once

#include <string>
#include <glad/gl.h>

#include "../graphics.hpp"

struct FrameBufferData {
    GLuint vertex_array_object_id{DUMMY_ID};
    GLuint vertex_buffer_object_id{DUMMY_ID};
    GLuint frame_buffer_object_id{DUMMY_ID};
    GLuint color_render_buffer_object_id{DUMMY_ID};
    GLuint depth_render_buffer_object_id{DUMMY_ID};
    GLuint color_attachment_id{DUMMY_ID};
    GLuint depth_attachment_id{DUMMY_ID};

    static auto constexpr DUMMY_ID = GLuint{0};

    FrameBufferData() = default;

    ~FrameBufferData();

    FrameBufferData(FrameBufferData&) = delete;

    inline FrameBufferData(FrameBufferData&& other) noexcept
    : vertex_array_object_id{other.vertex_array_object_id}
    , vertex_buffer_object_id{other.vertex_buffer_object_id}
    , frame_buffer_object_id{other.frame_buffer_object_id}
    , color_render_buffer_object_id{other.color_render_buffer_object_id}
    , depth_render_buffer_object_id{other.depth_render_buffer_object_id}
    , color_attachment_id{other.color_attachment_id}
    , depth_attachment_id{other.depth_attachment_id} {
        other.vertex_array_object_id = DUMMY_ID;
        other.vertex_buffer_object_id = DUMMY_ID;
        other.frame_buffer_object_id = DUMMY_ID;
        other.color_render_buffer_object_id = DUMMY_ID;
        other.depth_render_buffer_object_id = DUMMY_ID;
        other.color_attachment_id = DUMMY_ID;
        other.depth_attachment_id = DUMMY_ID;
    }

    auto operator=(this FrameBufferData&, FrameBufferData&)
        -> FrameBufferData& = delete;

    inline auto operator=(
        this FrameBufferData& self, FrameBufferData&& other
    ) noexcept -> FrameBufferData& {
        self.vertex_array_object_id = other.vertex_array_object_id;
        self.vertex_buffer_object_id = other.vertex_buffer_object_id;
        self.frame_buffer_object_id = other.frame_buffer_object_id;
        self.color_render_buffer_object_id =
            other.color_render_buffer_object_id;
        self.depth_render_buffer_object_id =
            other.depth_render_buffer_object_id;
        self.color_attachment_id = other.color_attachment_id;
        self.depth_attachment_id = other.depth_attachment_id;

        other.vertex_array_object_id = DUMMY_ID;
        other.vertex_buffer_object_id = DUMMY_ID;
        other.frame_buffer_object_id = DUMMY_ID;
        other.color_render_buffer_object_id = DUMMY_ID;
        other.depth_render_buffer_object_id = DUMMY_ID;
        other.color_attachment_id = DUMMY_ID;
        other.depth_attachment_id = DUMMY_ID;

        return self;
    }
};

class FrameBuffer {
    friend class SceneHandler;
    std::string vName, fName;

    tmine::ShaderProgram screenShader;
    static float screenQuad[24];

    std::shared_ptr<FrameBufferData> data;

    int msaa;
    int width, height;

public:
    FrameBuffer(
        std::string const& vName, std::string const& fName,
        glm::uvec2 window_size
    );
    void bind();
    void draw();
    void drawDepth();
    void drawBoth();
    void bindColorTex();
    void bindDepthTex();
    void refreshShader();
    int check();
    static void unbind();
};
