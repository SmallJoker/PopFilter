#include "audio.h"
#include "headers.h"
#include <cstring>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

// https://xiph.org/vorbis/doc/vorbisfile/reference.html

// FFT: ?
// https://github.com/mborgerding/kissfft/blob/master/test/testcpp.cc

Audio::Audio(const std::string &path)
{
	m_file = new OggVorbis_File;
	ASSERT(ov_fopen(path.c_str(), m_file) >= 0);

	vorbis_info *vi = ov_info(m_file, -1);
	LOG("Audio meta:"
		<< "\n\tChannels:   " << vi->channels
		<< "\n\tFrequency:  " << getSampleRate() << " Hz"
		<< "\n\tPCM Length: " << getSamples() << " ("
		<< (getSamples() / getSampleRate()) << "s)"
	);
}

Audio::~Audio()
{
	ov_clear(m_file);
	delete m_file;
	LOG("Closing audio");
}

int64_t Audio::getSampleRate() { return ov_info(m_file, -1)->rate; }
int64_t Audio::getSamples() { return ov_pcm_total(m_file, -1); }
double Audio::getTime() { return ov_time_tell(m_file); }
void   Audio::setTime(double pos) { ov_time_seek(m_file, pos); }
int64_t Audio::getPosition() { return ov_raw_tell(m_file); }
void    Audio::setPosition(int64_t pos) { ov_raw_seek(m_file, pos); }

int16_t *Audio::readS16(int64_t &samples)
{
	int16_t *buffer = new int16_t[samples];
	int64_t bytes_to_read = samples * sizeof(int16_t);

	int dummy;
	int64_t progress;
	for (progress = 0; progress < bytes_to_read;) {
		long status = ov_read(m_file, ((char *)buffer) + progress,
				bytes_to_read - progress, 0, 2, 1, &dummy);

		if (status == 0)
			break; // EOF

		ASSERT(status >= 0);
		progress += status;

	}
	samples = progress / sizeof(int16_t);

	LOG("Read " << progress << " / " << bytes_to_read << " bytes");
	return buffer;
}

float *Audio::readF32(int64_t &samples)
{
	float *buffer = new float[samples];

	int dummy;
	int64_t progress;
	float **pcm;
	for (progress = 0; progress < samples;) {
		long status = ov_read_float(m_file, &pcm,
				samples - progress, &dummy);

		if (status == 0)
			break; // EOF

		ASSERT(status > 0);
		for (int i = 0; i < status; ++i) 
			*(buffer + progress + i) = pcm[0][i];

		progress += status;
	}

	LOG("Read " << progress << " / " << samples << " samples");
	return buffer;
}
