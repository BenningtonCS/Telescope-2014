#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
// version for only one thread
// Notes - float is faster intel_ipps is even faster


void fft_init(int n, fftwf_plan * p, float **reamin0, float **reamout0)
{
    fftwf_complex *in, *out;
//printf("entering fft_init\n");
    in = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * n);
    out = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * n);
// printf("entering fft_init after malloc %p\n",in);
    *p = fftwf_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    *reamin0 = (float *) in;
    *reamout0 = (float *) out;

// printf("reamin0 %p reamout0 %p\n",reamin0,reamout0);
}

void fft_free(fftwf_plan * p, float **reamin0, float **reamout0)
{
    fftwf_destroy_plan(*p);
    fftwf_free((fftwf_complex *) * reamin0);
    fftwf_free((fftwf_complex *) * reamout0);
}

void cfft(fftwf_plan * p)
{
    fftwf_execute(*p);
}
