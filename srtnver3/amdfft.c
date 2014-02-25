#include "acml.h"
#include <stdio.h>
#include <stdlib.h>


void fft_init(int n, float ream[], double *comm, int *info)
{
    complex *xx, *com;
    /* Allocate communication work array */
//  comm = (double *)malloc((5*n+100)*sizeof(double));
    com = (complex *) comm;
    xx = (complex *) ream;
    /* Initialize communication work array */
    cfft1d(0, n, xx, com, info);
}


void cfft(int n, float ream[], double *comm, int *info)
{
    complex *xx, *com;
    com = (complex *) comm;
    xx = (complex *) ream;
    cfft1d(-1, n, xx, com, info);
}
