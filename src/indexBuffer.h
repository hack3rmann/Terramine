#pragma once

class iBuffer {
private:
    unsigned int m_RendererID;
    unsigned int m_Count;

public:
    iBuffer(unsigned int const* data, unsigned int count);
    ~iBuffer();
    void bind() const;
    void unbind() const;
    inline unsigned int getCount() const;
};
