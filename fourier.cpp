#include "fourier.h"
#include "util/timer.h"
#include <cmath>
#include <cstdio>

bool cosine_lut_initialized = false;
const size_t COSINE_LUT_SIZE = 512;
// Consine values from 0° to 90°
float cosine_LUT[COSINE_LUT_SIZE];

void initSineLUT()
{
	if (cosine_lut_initialized)
		return;

	// Cache 90°
	float inv = 1.0f / (float)COSINE_LUT_SIZE;
	for (size_t i = 0; i < COSINE_LUT_SIZE; ++i)
		cosine_LUT[i] = std::cos(M_PI * 0.5f * i * inv);

	cosine_lut_initialized = true;
}

inline float getFastCosine(float ratio)
{
	static float factor;
	// Fit "ratio" into cosine_LUT (0 ... COSINE_LUT_SIZE)

	// 1) Exactly one period
	ratio -= (int)ratio; // 0.0f <= ratio < 1.0f

	// 2) Mirror 90° segments to fit into the (0° .. 90° LUT)
	if (ratio < 0.25f) {
		factor = 1;
	} else if (ratio < 0.5f) {
		ratio = 0.5f - ratio;
		factor = -1;
	} else if (ratio < 0.75f) {
		ratio -= 0.5f;
		factor = -1;
	} else {
		ratio = 1.0f - ratio;
		factor = 1;
	}

	// 3) Upscale ratio to index the array
	ratio *= 4 * COSINE_LUT_SIZE;
	return factor * cosine_LUT[(size_t)(ratio + 0.5f)];
}

size_t FFT_1Dfast(float *din, float *dout, size_t n)
{
	initSineLUT();
	float inv_n = 1.0f / n;
	float real;
	size_t i, t;
	Timer timer("FFT_1Dfast");

	dout[0] = 0;
	size_t arr_i = 1;
    for (i = 1; i < n / 2; i <<= 1) {
		real = 0.0f;
        for (t = 0; t < n; ++t)
            real += din[t] * getFastCosine(t * i * inv_n);

		dout[arr_i++] = real * inv_n;
	}
	return arr_i;
}

size_t DFT_1Dfast(float *din, float *dout, size_t n)
{
	initSineLUT();
	float inv_n = 1.0f / n;
	float real;
	size_t i, t;
	Timer timer("DFT_1Dfast");

    for (i = 0; i < n / 2; ++i) {
		if ((i & 0x3FF) == 0x3FF)
			timer.tick();

		real = 0.0f;
        for (t = 0; t < n; ++t)
			real += din[t] * getFastCosine(t * i * inv_n);

		dout[i] = real * inv_n;
	}
	return i;
}

size_t DFT_1Dstd(float *din, float *dout, size_t n)
{
	float inv_n = 1.0f / n;
	float angle;
	size_t i, t;
	Timer timer("DFT_1Dstd");
	
	dout[0] = 0;

    for (i = 1; i < n / 2; ++i) {
		float real = 0.0f;
		//float imag = 0.0f;
		if ((i & 0x3FF) == 0x3FF)
			timer.tick();

        for (t = 0; t < n; ++t) {
            angle = 2.0f * (float)M_PI * t * i * inv_n;
            real += din[t] * std::cos(angle);
            //imag += -din[t] * std::sin(angle);
        }
        dout[i] = real * inv_n;
		//dout[i] = std::hypot(real, imag) / n;
    }
    return i;
}
