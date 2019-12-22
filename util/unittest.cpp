#include "headers.h"
#include "ringbuffer.h"
#include <chrono>
#include <mutex>
#include <string.h>
#include <thread>

static std::mutex lock_me;

void buffer_write(RingBuffer<wchar_t> *buf, const wchar_t *what, size_t len)
{
	for (int i = 0; i < 50; i++) {
		lock_me.lock();
		buf->write(what, 0, len);
		lock_me.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	LOG("Done");
}

void buffer_read(RingBuffer<wchar_t> *buf, wchar_t *what, size_t len)
{
	for (int i = 0; i < 50; i++) {
		lock_me.lock();
		buf->read(what, 0, len);
		lock_me.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(11));
	}
	LOG("Done");
}

void testRingBuffer();

void runUnittest()
{
	LOG("Started unittests");
	testRingBuffer();

	{ // Thread
		RingBuffer<wchar_t> buffer(1000);

		const wchar_t long_i[] = L"This is a very long, wide text like the lorem ipsum";
		const size_t len = wcslen(long_i);
		wchar_t *long_o = new wchar_t[len]; 
		LOG("Thread read/write, sample length = " << len);

		std::thread write_thread(&buffer_write, &buffer, long_i, len);
		std::thread read_thread(&buffer_read, &buffer, long_o, len);

		write_thread.join();
		read_thread.join();
	}
	LOG("Unittest passed");
	std::cout << "---------------------------------" << std::endl;
}

// Thanks StackOverflow! https://stackoverflow.com/questions/3381614/
std::string string_to_hex(const std::string &input)
{
    static const char * const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i) {
        const uint8_t c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

void testRingBuffer()
{
	RingBuffer<char> buffer(30);
	const char short_i[] = "pink ponies";
	const size_t len = strlen(short_i);
	char *short_o = new char[len + 1];
	short_o[len] = 0; // manual NULL termination

	for (int i = 0; i < 10; ++i) {
		buffer.write(short_i, 0, len);
		buffer.read(short_o, 0, len);

		ASSERT(memcmp(short_i, short_o, sizeof(char) * len) == 0);
		ASSERT(buffer.getUsed() == 0);

		memset(short_o, 101, sizeof(char) * len);
	}

	// Is empty now. Read parts
	buffer.write(short_i, 0, len);
	const size_t read_length = 3;
	for (int i = 0; i < len - read_length; i += read_length) {
		buffer.read(short_o, 0, read_length);
		ASSERT(memcmp(short_i + i, short_o,
			sizeof(char) * read_length) == 0);
	}

	// TODO: Test return values (overflow)

	LOG("Passed");
}
