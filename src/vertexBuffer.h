#pragma once

class vBuffer {
private:
    unsigned int m_RendererID;

public:
    vBuffer(void const* data, unsigned int size);
    ~vBuffer();
    void bind();
    void unbind();
};
