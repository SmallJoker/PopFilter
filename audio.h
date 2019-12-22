#pragma once

#include <cstdint>
#include <string>

struct OggVorbis_File;

class Audio {
public:
	Audio(const std::string &path);
	~Audio();

	int64_t getSampleRate();
	int64_t getSamples();
	// Position getter/setter functions
	double getTime();
	void   setTime(double pos);
	int64_t getPosition();
	void    setPosition(int64_t pos);

	// Audio operations
	int16_t *readS16(int64_t &samples);
	float   *readF32(int64_t &samples);
private:
	OggVorbis_File *m_file;
};
