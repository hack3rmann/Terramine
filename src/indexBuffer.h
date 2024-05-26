#pragma once
#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

class iBuffer {
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	iBuffer(const unsigned int* data, unsigned int count);
	~iBuffer();
	void bind() const;
	void unbind() const;
	inline unsigned int getCount() const;
};

#endif // !INDEX_BUFFER_H
