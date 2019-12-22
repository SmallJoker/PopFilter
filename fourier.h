#pragma once

#include <cstddef>

size_t FFT_1Dfast(float *din, float *dout, size_t n);
size_t DFT_1Dfast(float *din, float *dout, size_t n);
size_t DFT_1Dstd(float *din, float *dout, size_t n);
