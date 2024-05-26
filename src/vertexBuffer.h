#pragma once
#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

class vBuffer {
private:
	unsigned int m_RendererID;
public:
	vBuffer(const void* data, unsigned int size);
	~vBuffer();
	void bind();
	void unbind();
};

#endif // !VERTEX_BUFFER_H
