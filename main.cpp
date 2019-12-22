#include "audio.h"
#include "fourier.h"
#include "headers.h"
#include "util/args_parser.h"

#include <array>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>

extern void runUnittest();

void normalize(float *data, size_t n)
{
	float total = 0.0f;
	for (size_t i = 0; i < n; ++i)
		total += data[i];

	total = n / total;

	for (size_t i = 0; i < n; ++i)
		data[i] *= total;
}

void smooth(float *data, size_t n, float v)
{
	float avg = data[0];
	for (size_t i = 0; i < n; ++i) {
		if (data[i] > 0)
			avg += (data[i] - avg) * v;
		data[i] = avg;
	}
}

int main(int argc, char **argv)
{
	CLIArgF64 ca_start("s", 0.0);
	CLIArgF64 ca_duration("d", 0.03);
	CLIArgStr ca_file("f", "test.ogg");
	CLIArg::parseArgs(argc, argv);

	//runUnittest();

	Audio audio(ca_file.get());
	audio.setTime(ca_start.get());
	LOG("time: " << audio.getTime());

	const int64_t DEFAULT_LEN = audio.getSampleRate() * ca_duration.get();

	int64_t samples = DEFAULT_LEN;
	size_t f_samples = samples / 2;
	float fourier[f_samples];

	// Read audio and do DFT
	{
		float *buffer = audio.readF32(samples);

		f_samples = DFT_1Dfast(buffer, fourier, samples);

		// Compensate ear
		for (size_t i = 0; i < f_samples; ++i)
			fourier[i] *= std::sqrt((i) * audio.getSampleRate() / (float)samples);

		delete buffer;
	}

	normalize(fourier, f_samples);
	//smooth(fourier, f_samples, 0.02f);

	LOG("time: " << audio.getTime());

	// Dump to file
	{
		std::ofstream file;
		file.open("plot_data.dat", std::ios::trunc);
		char buf[20];
		for (size_t i = 0; i < f_samples; ++i) {
			sprintf(buf, "%0.3e\n", fourier[i]);
			file << std::string(buf);
		}
		file.close();
	}

	// Analyze peaks
	{
		struct PeakData {
			float freq;
			float gain;
		};
		std::vector<PeakData> sorted;
		for (size_t i = 0; i < f_samples; ++i) {
			if (std::abs(fourier[i]) < 3.0f)
				continue;
			PeakData pd;
			pd.freq = (i) * audio.getSampleRate() / (float)samples;
			pd.gain = fourier[i];
			sorted.emplace_back(pd);
		}
		std::sort(sorted.begin(), sorted.end(),
			[=] (const PeakData &a, const PeakData &b) -> bool {
				return a.gain > b.gain;
		});
		int n = 0;
		for (const PeakData &pd : sorted) {
			printf("Peak at %0.2f Hz (%0.2f)\n", pd.freq, pd.gain);
			if (++n >= 5)
				break;
		}
	}
	return 0;
}
