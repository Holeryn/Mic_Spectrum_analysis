#ifndef TRANSFORM
#define TRANSFORM

#include <math.h>
#include <stdlib.h>
#include <complex.h>
#include "portaudio.h"

#define DEBUG 0

// Fast fourier transform of a given amplidtude spectrum
// Input: Amplitude spectrum
// PS: SAMPLE = float
void transform(float *amplitudesS, float *frequencyS,int N);


#endif
