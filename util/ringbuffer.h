#pragma once

#include <cstdint>
#include <cstddef>
#include <cwchar>

template <typename T>
class RingBuffer {
public:
	RingBuffer(size_t capacity);
	~RingBuffer();

	bool write(const T &dat);
	bool write(const T *dat, size_t start, size_t end);
	bool read(T &dat);
	bool read(T *dat, size_t start, size_t end);
	inline size_t getCapacity() const
	{
		return m_end - m_buffer;
	}
	inline size_t getUsed() const
	{
		if (m_head > m_tail)
			return getCapacity() - (m_head - m_tail);
		return m_tail - m_head;
	}
	inline void readClear()
	{
		m_tail = m_head;
	}
private:
	T *m_head = nullptr;
	T *m_tail = nullptr;
	T *m_buffer = nullptr;
 	T *m_end = nullptr;
};

template class RingBuffer<int16_t>;
template class RingBuffer<char>;
template class RingBuffer<wchar_t>;
