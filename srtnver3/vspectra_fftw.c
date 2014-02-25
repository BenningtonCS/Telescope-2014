#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/io.h>
#include <math.h>
#include <string.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <fftw3.h>

#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"
#include "rtl-sdr.h"


#define NSAM 0x100000


static rtlsdr_dev_t *dev = NULL;

void fft_init(int, fftwf_plan *, float **, float **);
void fft_free(fftwf_plan *, float **, float **);
void cfft(fftwf_plan *);

void vspectra(void)
{

    int i, j, k, kk, num, numm, i4, maxi, r;
    int m, mm, blsiz, blsiz2, nsam, n_read;
    double avsig, av, max, min, noise, wid;
    uint8_t *bufferRead = malloc((NSAM) * sizeof(uint8_t));
    static double vspec[NSPEC];
    static int wtt[NSPEC];
    static double re[NSPEC * 2], am[NSPEC * 2];
    double smax;
    fftwf_plan p0;
    float *reamin0, *reamout0;


    blsiz = NSPEC * 2;
    blsiz2 = blsiz / 2;
    d1.bw = 2.4;                // fixed 2.4 for TV dongle 10 MHz for ADC card
    if (d1.fbw == 0.0)
        d1.fbw = 2.0;           //  use bandwidth default if not set in srt.cat
    d1.f1 = 0.5 - (d1.fbw / d1.bw) * 0.5;
    d1.f2 = 0.5 + (d1.fbw / d1.bw) * 0.5;
    d1.fc = (d1.f1 + d1.f2) * 0.5;
    d1.lofreq = 0;              // not used but needs to be set to zero to flag the use of the dongle 
    d1.efflofreq = d1.freq - d1.bw * 0.5;

    if (!d1.fftsim) {
        fft_init(blsiz2, &p0, &reamin0, &reamout0);
    }

    num = d1.nblk;                    //was 20   // was 100
    nsam = NSAM;
    d1.nsam = NSAM * num;
    avsig = 0;
    numm = 0;
    smax = 0;
    max = -1e99;
    min = 1e99;
    for (i = 0; i < blsiz2; i++)
        vspec[i] = 0.0;
    for (k = 0; k < num; k++) {
        if (!d1.radiosim)
// Read the raw data from the RTL Dongle
            r = rtlsdr_read_sync(dev, bufferRead, nsam, &n_read);
        else {
            av = 5.0;
            if (d1.elnow < 5.0)
                av = av * (d1.tsys + d1.tcal) / d1.tsys;
            if (strstr(soutrack, "Sun")) {
                av = sqrt(d1.eloff * d1.eloff +
                          d1.azoff * d1.azoff * cos(d1.elnow * PI / 180.0) * cos(d1.elnow * PI / 180.0) +
                          1e-6);
                if (av > d1.beamw)
                    av = d1.beamw;
                av = 5.0 + 25.0 * cos(av * PI * 0.5 / d1.beamw) * cos(av * PI * 0.5 / d1.beamw);
            }
            for (i = 0; i < nsam; i++)
                bufferRead[i] = (uint8_t) (sqrt(av) * gauss() + 127.397 + 0.5 + 0 * sin(2.0 * PI * 0.5 * (i / 2) / d1.bw)); // simulate data
        }

//   for(i=0;i<nsam;i+=0x10000)  printf("%d %f\n",i,(double)(bufferRead[i]-127.397));
        for (kk = 0; kk < nsam / blsiz; kk++) {
            if (d1.fftsim)
                for (i = 0; i < blsiz2; i++) {
                    re[i] = (double) (bufferRead[2 * i + kk * blsiz] - 127.397);
                    am[i] = (double) (bufferRead[2 * i + 1 + kk * blsiz] - 127.397);
                    if (re[i] > smax)
                        smax = re[i];
            } else
                for (i = 0; i < blsiz2; i++) {
                    reamin0[2 * i] = (float) (bufferRead[2 * i + kk * blsiz] - 127.397);
                    reamin0[2 * i + 1] = (float) (bufferRead[2 * i + 1 + kk * blsiz] - 127.397);
                    if (reamin0[2 * i] > smax)
                        smax = reamin0[2 * i];
                }

            if (d1.fftsim)
                Four(re, am, blsiz2);
            else {
                cfft(&p0);
                for (i = 0; i < blsiz2; i++) {
                    re[i] = reamout0[2 * i];
                    am[i] = reamout0[2 * i + 1];
                }
            }
// for(i = 0; i < blsiz2; i++) if(re[i] > max) max=re[i];
// for(i = 0; i < blsiz2; i++) if(re[i] < min) min=re[i];
            for (i = 0; i < blsiz2; i++) {
                if (i < blsiz2 / 2)
                    j = i + blsiz2 / 2;
                else
                    j = i - blsiz2 / 2;
                vspec[j] += re[i] * re[i] + am[i] * am[i];
                numm++;
            }
        }
    }
//   printf("max %f min %f\n",max,min);
    max = av = 0;
    maxi = 0;
    for (i = 0; i < blsiz2; i++)
        wtt[i] = 1;
    if (numm > 0) {
        if (d1.nfreq == blsiz2) {
            for (i = 0; i < blsiz2; i++) {
                if (i > 10)
                    spec[i] = vspec[i] / (double) numm;
                else
                    spec[i] = 0;
            }
        } else {
            m = blsiz2 / d1.nfreq;
            for (i = 0; i < d1.nrfi; i++) {
                i4 = (d1.rfi[i] - d1.freq + d1.bw * 0.5) * blsiz2 / d1.bw + 0.5; // index of rfi MHz 
                wid = 0.5 * d1.rfiwid[i] / (d1.bw / NSPEC);
                for (j = -wid; j <= wid; j++)
                    if ((i4 + j) >= 0 && (i4 + j) < blsiz2)
                        wtt[i4 + j] = 0;
            }
            for (j = 0; j < d1.nfreq; j++) {
                av = mm = 0;
                for (i = j * m - m / 2; i <= j * m + m / 2; i++) {
                    if (i > 10 && i < blsiz2 && wtt[i]) { // wtt=0 removal of spurs
                        av += vspec[i] / (double) numm;
                        if (vspec[i] > max) {
                            max = vspec[i];
                            maxi = i;
                        }
                        mm++;
                    }
                }
                if (mm > 0)
                    spec[j] = av / mm;
                else {
                    spec[j] = 0;
                    if (j > 10)
                        printf("check RFI settings in srt.cat data deleted at %8.3f\n",
                               j * d1.bw / d1.nfreq + d1.freq - d1.bw * 0.5);
                }
            }
            max = max / (double) numm;
            noise = spec[maxi / m] * sqrt(2.0 * blsiz2 / (double) d1.nsam);
            if (max > spec[maxi / m] + d1.rfisigma * noise && d1.printout) // rfisigma sigma
                printf("check for RFI at %8.4f MHz max %5.0e av %5.0e smax %5.0f %3.0f sigma\n",
                       maxi * d1.bw / blsiz2 + d1.freq - d1.bw * 0.5, max, spec[maxi / m], smax,
                       (max - spec[maxi / m]) / noise);
        }
    }
    d1.smax = smax;
    if (!d1.fftsim)
        fft_free(&p0, &reamin0, &reamout0);
    if (!d1.radiosim)
        free(bufferRead);
}

// Initilizes and Opens the RTL Dongle
void Init_Device(int mode)
{
    int r;
    int i = 0;
    int gain = 496;             //496;//0;//125;//386;
    /*
       { 0, 9, 14, 27, 37, 77, 87, 125, 144, 157,
       166, 197, 207, 229, 254, 280, 297, 328,
       338, 364, 372, 386, 402, 421, 434, 439,
       445, 480, 496 };
     */

    uint32_t dev_index = 0;
    uint32_t frequency = (d1.freq + d1.freqcorr) * 1e6; //1433170000;//1420400000;//1409246000;//1407630000;//34000000;//1420400000;//1421550000;//99500000;//100000000;
    uint32_t samp_rate = 2.4 * 1e6 + 0.5; //1400000;//2400000;//DEFAULT_SAMPLE_RATE;
    int device_count;
    char vendor[256], product[256], serial[256];
    d1.dongle = 1;

    if (mode) {
        /* Set the frequency */
        r = rtlsdr_set_center_freq(dev, frequency);
        if (r < 0)
            fprintf(stderr, "WARNING: Failed to set center freq.\n");
        else if (d1.printout)
            printf("Tuned to %u Hz.\n", frequency);
        return;
    }

    device_count = rtlsdr_get_device_count();
    if (!device_count) {
        fprintf(stderr, "No supported devices found.\n");
        exit(1);
    }

    if (d1.printout)
        printf("Found %d device(s):\n", device_count);
    for (i = 0; i < device_count; i++) {
        rtlsdr_get_device_usb_strings(i, vendor, product, serial);
        if (d1.printout)
            printf("  %d:  %s, %s, SN: %s\n", i, vendor, product, serial);
    }

    if (d1.printout)
        printf("Using device %d: %s\n", dev_index, rtlsdr_get_device_name(dev_index));

    r = rtlsdr_open(&dev, dev_index);
    if (r < 0) {
        fprintf(stderr, "Failed to open rtlsdr device #%d.\n", dev_index);
        exit(1);
    }

    /* Set the sample rate */
    r = rtlsdr_set_sample_rate(dev, samp_rate);
    if (r < 0)
        fprintf(stderr, "WARNING: Failed to set sample rate.\n");

    /* Set the frequency */
    r = rtlsdr_set_center_freq(dev, frequency);
    if (r < 0)
        fprintf(stderr, "WARNING: Failed to set center freq.\n");
    else if (d1.printout)
        printf("Tuned to %u Hz.\n", frequency);

    if (0 == gain) {
        /* Enable automatic gain */
        r = rtlsdr_set_tuner_gain_mode(dev, 0);
        if (r < 0)
            fprintf(stderr, "WARNING: Failed to enable automatic gain.\n");
    } else {
        /* Enable manual gain */
        r = rtlsdr_set_tuner_gain_mode(dev, 1);
        if (r < 0)
            fprintf(stderr, "WARNING: Failed to enable manual gain.\n");

        /* Set the tuner gain */// doesn't set gain to gain value - uses R828_Arry[0] = (R828_Arry[0] & 0xF0) | 0x0F;//0x06;//gLNA;//lna_index
        r = rtlsdr_set_tuner_gain(dev, gain);
        if (r < 0)
            fprintf(stderr, "WARNING: Failed to set tuner gain.\n");
    }

    r = rtlsdr_reset_buffer(dev);
//    printf("Reset Status: %u \n", r);

}
