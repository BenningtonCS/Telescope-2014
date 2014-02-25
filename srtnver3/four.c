#include <stdio.h>
#include <math.h>
#define PI 3.1415926536


void Four(double *fft_r, double *fft_i, int nn)
{
    int n, mmax, m, j, istep, i, ii, jj;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
        if (j > i) {
            jj = (j - 1) / 2;
            ii = (i - 1) / 2;
            tempr = fft_r[jj];
            tempi = fft_i[jj];
            fft_r[jj] = fft_r[ii];
            fft_i[jj] = fft_i[ii];
            fft_r[ii] = tempr;
            fft_i[ii] = tempi;
        }
        m = n >> 1;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax = 2;
    while (n > mmax) {
        istep = mmax << 1;

        theta = -(6.28318530717959 / mmax);
        wtemp = sin(0.5 * theta);
        wpr = -2.0 * wtemp * wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 1; m < mmax; m += 2) {
            for (i = m; i <= n; i += istep) {
                j = i + mmax;
                jj = (j - 1) / 2;
                ii = (i - 1) / 2;
                tempr = wr * fft_r[jj] - wi * fft_i[jj];
                tempi = wr * fft_i[jj] + wi * fft_r[jj];
                fft_r[jj] = fft_r[ii] - tempr;
                fft_i[jj] = fft_i[ii] - tempi;
                fft_r[ii] += tempr;
                fft_i[ii] += tempi;
            }
            wr = (wtemp = wr) * wpr - wi * wpi + wr;
            wi = wi * wpr + wtemp * wpi + wi;
        }
        mmax = istep;
    }

}
