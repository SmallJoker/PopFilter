/*
Ring buffer class
	Version 1.0a

*/
#include "ringbuffer.h"
#include <string.h> // memcpy

template <typename T>
RingBuffer<T>::RingBuffer(size_t capacity)
{
	if (capacity < 1)
		throw "Capacity too small";

	m_buffer = new T[capacity];
	m_end = m_buffer + capacity;
	m_head = m_buffer;
	m_tail = m_buffer;
}

template <typename T>
RingBuffer<T>::~RingBuffer()
{
	delete m_buffer;
}


template <typename T>
bool RingBuffer<T>::write(const T &dat)
{
	*m_head = dat;

	if (++m_head >= m_end)
		m_head = m_buffer;

	return (m_head == m_tail); // overflow?
}

template <typename T>
bool RingBuffer<T>::write(const T *dat, size_t start, size_t end)
{
	size_t count = end - start;
	if (count > getCapacity())
		throw "Write size exceeds buffer size";

	size_t free_until_wrap = m_end - m_head;
	//const T *old_head = m_head;

	if (m_tail > m_head && (size_t)(m_tail - m_head) < count)
		return true;
	if (m_head > m_tail && getCapacity() - (m_head - m_tail) < count)
		return true;

	if (count <= free_until_wrap) {
		// Single copy operation (no wrap needed)
		memcpy(m_head, dat + start, sizeof(T) * count);
		m_head += count;

		if (m_head >= m_end)
			m_head = m_buffer;

		//return (old_head < m_tail && m_head >= m_tail); // easiest overflow case

		return false;
	}

	// Two copy operations
	memcpy(m_head, dat + start, sizeof(T) * free_until_wrap);
	count -= free_until_wrap;
	memcpy(m_buffer, dat + start + free_until_wrap, sizeof(T) * count);
	m_head = m_buffer + count;

	//return old_head < m_tail; // wrap happened. Tail cannot be anywhere else

	return false;
}


template <typename T>
bool RingBuffer<T>::read(T &dat)
{
	dat = *m_tail;

	if (++m_tail >= m_end)
		m_tail = m_buffer;

	return (m_head == m_tail); // overflow?
}

template <typename T>
bool RingBuffer<T>::read(T *dat, size_t start, size_t end)
{
	size_t count = end - start;
	if (count > getCapacity())
		throw "Read size exceeds buffer size";

	size_t free_until_wrap = m_end - m_tail;
	//const T *old_tail = m_tail;
	
	if (m_head == m_tail)
		return true;
	if (m_head > m_tail && (size_t)(m_head - m_tail) < count)
		return true;
	if (m_tail > m_head && getCapacity() - (m_tail - m_head) < count)
		return true;

	if (count <= free_until_wrap) {
		// Single copy operation (no wrap needed)
		memcpy(dat + start, m_tail, sizeof(T) * count);
		m_tail += count;

		if (m_tail >= m_end)
			m_tail = m_buffer;

		//return (old_tail < m_head && m_tail >= m_head); // easiest overflow case

		return false;
	}

	// Two copy operations
	memcpy(dat + start, m_tail, sizeof(T) * free_until_wrap);
	count -= free_until_wrap;
	memcpy(dat + start + free_until_wrap, m_buffer, sizeof(T) * count);
	m_tail = m_buffer + count;

	//return old_tail < m_head; // wrap happened. Tail cannot be anywhere else

	return false;
}
