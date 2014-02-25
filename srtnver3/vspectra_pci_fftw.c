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
#include "pci-das4020.h"


int ADC_Mode = ADC_DMA_CONVERSION;
int Count = 1;
int Board = 0;
int Channel = 0;
int dmamap = 0;
int Status;
int freq_A2D = 20000000;
unsigned short *readBuff;

int fdDAC0, fdDAC1;             /* D/A file descriptors */
int fdADC, fdADC0, fdADC1, fdADC2, fdADC3; /* A/D file descriptors */
int fdDIOA, fdDIOB, fdDIOC;     /* DIO file descriptors */


static unsigned short buffer1[0x200000];

void fft_init(int, fftwf_plan *, float **, float **);
void fft_free(fftwf_plan *, float **, float **);
void cfft(fftwf_plan *);


void DoOpenDevices();
int get_pci(unsigned short value[], int count);

void Init_Device(int mode)
{
    DoOpenDevices();
    mode = 0;
}

void vspectra(void)
{

    int i, j, size, sizep, kk, kkk, num, numm, i4;
    int k, m, mm, blsiz, blsiz2, nsam, maxi;
    double avsig, av, wid, noise, max;
    static double vspec[NSPEC];
    static int wtt[NSPEC];
    static double re[NSPEC * 2], am[NSPEC * 2];
    double smax, aam, rre, aam2, rre2;
    fftwf_plan p0;
    float *reamin0, *reamout0;


    d1.bw = 10.0;               // 10 MHz
    d1.lofreq = 1416.0;         // Luff 1416 MHz
    d1.efflofreq = d1.lofreq;
    d1.f1 = (d1.freq - d1.lofreq) / d1.bw - (d1.fbw / d1.bw) * 0.5;
    d1.f2 = (d1.freq - d1.lofreq) / d1.bw + (d1.fbw / d1.bw) * 0.5;
    d1.fc = (d1.f1 + d1.f2) * 0.5;

    blsiz = NSPEC * 2;

    if (!d1.fftsim) {
        fft_init(blsiz, &p0, &reamin0, &reamout0);
    }
//    printf("comm %x\n",comm);

    nsam = 0x100000;
    blsiz2 = blsiz / 2;
    num = 20;                   // was 100
    d1.nsam = nsam * num;
    avsig = 0;
    size = 0;
    numm = 0;
    smax = 0;
    av = 0;
    for (i = 0; i < blsiz2; i++)
        vspec[i] = 0.0;
    size = -1;
    if (!d1.radiosim)
        while (size != nsam)
            size = get_pci(buffer1, nsam); // wait for transfer to complete
    else {
        av = 5.0;
        if (d1.elnow < 5.0)
            av = av * (d1.tsys + d1.tcal) / d1.tsys;
        if (strstr(soutrack, "Sun")) {
            av = sqrt(d1.eloff * d1.eloff +
                      d1.azoff * d1.azoff * cos(d1.elnow * PI / 180.0) * cos(d1.elnow * PI / 180.0) + 1e-6);
            if (av > d1.beamw)
                av = d1.beamw;
            av = 5.0 + 25.0 * cos(av * PI * 0.5 / d1.beamw) * cos(av * PI * 0.5 / d1.beamw);
        }

        for (i = 0; i < nsam; i++)
            buffer1[i] = 2048 + sqrt(av) * gauss();
        size = nsam;
    }                           // simulate transfer
    sizep = size;
    for (k = 0; k < num; k++) {
        if (k < num - 1) {
            if (!d1.radiosim)
                size = get_pci(buffer1, nsam); // start new transfer
            else {
                for (i = 0; i < nsam; i++)
                    buffer1[i] = 2048 + sqrt(av) * gauss();
                size = nsam;
            }                   // simulate transfer
        }
        if (sizep == nsam) {    // work on previous buffer
            for (kk = 0; kk < sizep / blsiz; kk++) {
                avsig = 2048;   // should be 2048   
//                avsig = 2300;
                kkk = kk * blsiz;
                for (j = 0; j < blsiz; j++) {
//                      if(j==0 && kkk==0) printf("sam %f\n",(buffer1[j + kkk] & 0xfff) - avsig);
                    if (kk % 2 == 0)
                        reamin0[2 * j] = ((double) (buffer1[j + kkk] & 0xfff) - avsig);
                    else
                        reamin0[2 * j + 1] = ((double) (buffer1[j + kkk] & 0xfff) - avsig);
                    if (j && reamin0[2 * j] > smax)
                        smax = reamin0[2 * j];
                }
                if (kk % 2 == 1) {
                    if (d1.fftsim) {
                        for (i = 0; i < blsiz2; i++) {
                            re[i] = reamin0[2 * i];
                            am[i] = reamin0[2 * i + 1];
                        }
                        Four(re, am, blsiz2);
                    } else
                        cfft(&p0);

                    for (i = 0; i < blsiz2; i++) {
                        if (i >= 1) {
                            rre = reamout0[2 * i] + reamout0[2 * (blsiz - i)];
                            aam = reamout0[2 * i + 1] - reamout0[2 * (blsiz - i) + 1];
                            aam2 = -reamout0[2 * i] + reamout0[2 * (blsiz - i)];
                            rre2 = reamout0[2 * i + 1] + reamout0[2 * (blsiz - i) + 1];
                        } else {
                            rre = reamout0[2 * i] + reamout0[0];
                            aam = reamout0[2 * i + 1] - reamout0[1];
                            aam2 = -reamout0[2 * i] + reamout0[0];
                            rre2 = reamout0[2 * i + 1] + reamout0[1];
                        }
                        vspec[i] += rre * rre + aam * aam + rre2 * rre2 + aam2 * aam2;
                    }
                }
            }
            numm++;
        }
        while (size != nsam)
            if (!d1.radiosim)
                size = get_pci(buffer1, nsam); // wait for transfer to complete
            else {
                for (i = 0; i < nsam; i++)
                    buffer1[i] = 2048 + 10.0 * gauss();
                size = nsam;
            }                   // simulate transfer
        sizep = size;
    }
    av = max = 0;
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
                i4 = (d1.rfi[i] - d1.lofreq) * blsiz2 / d1.bw + 0.5; // index of rfi MHz 
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
                               j * d1.bw / d1.nfreq + d1.lofreq);
                }
            }
            max = max / (double) numm;
            noise = spec[maxi / m] * sqrt(2.0 * blsiz2 / (double) d1.nsam);
            if (max > spec[maxi / m] + d1.rfisigma * noise && d1.printout) // rfisigma sigma
                printf("check for RFI at %8.4f MHz max %5.0e av %5.0e smax %5.0f %3.0f sigma\n",
                       maxi * d1.bw / blsiz2 + d1.lofreq, max, spec[maxi / m], smax,
                       (max - spec[maxi / m]) / noise);
        }
    }
    d1.smax = smax;
    if (!d1.fftsim)
        fft_free(&p0, &reamin0, &reamout0);

}

void DoOpenDevices()
{
    char str[80];
    int *adc_fds[] = { &fdADC0 };
    int *dio_fds[] = { &fdDIOA };

    sprintf(str, "/dev/das4020-12/ad%d_%d", Board, Channel);
    if ((*adc_fds[0] = open(str, ADC_Mode | O_NONBLOCK)) < 0) {
        perror(str);
        printf("error opening device %s\n", str);
        exit(2);
    }
    ioctl(fdADC0, ADC_SET_GAINS, BP_1_00V);
    ioctl(fdADC0, ADC_SET_PACER_FREQ, freq_A2D);
    ioctl(fdADC0, ADC_SET_FIFO_SIZE, 0x8000); // default?

    sprintf(str, "/dev/das4020-12/dio%d_0%c", Board, 'A');
    if ((*dio_fds[0] = open(str, O_RDWR)) < 0) {
        perror(str);
        printf("error opening device %s\n", str);
        exit(2);
    }
    ioctl(fdDIOA, DIO_SET_DIRECTION, PORT_OUTPUT);
    fdADC = *adc_fds[0];
}


int get_pci(unsigned short value[], int count)
{
    int i;
    int bytesRead;
    unsigned int toggle = 0x0;

    Count = count;
    if (ADC_Mode == ADC_SOFT_CONVERSION) {
        toggle ^= 0x1;
        ioctl(fdADC, ADC_PSC_ENB, toggle);
        bytesRead = read(fdADC, value, Count);

        if (bytesRead != Count) {
            printf("testADC: Error on read() \n");
            printf("bytesRead = %d, and specified Count = %d\n", bytesRead, Count);
        }
    } else {
        if (!dmamap) {
            if ((readBuff = mmap(0, Count * 2, PROT_READ, MAP_PRIVATE, fdADC, 0 * getpagesize()))
                == (unsigned short *) MAP_FAILED) {
//                printf("Test Failed: Mmap call failed %x \n", (int) readBuff);
                printf("Test Failed: Mmap call failed %x \n", errno);
                printf(" %d\n", errno);
                sleep(3);
                return (0);
            } else {
                printf("Test Passed: Succesfully mmaped %d bytes\n", Count * 2);
            }
        }
        dmamap = 1;
        /* In the following read calls, the argument value will be ignored */
        /* Since we DMA to stuff over to the address held by readBuff     */

        bytesRead = read(fdADC, value, Count);

        if (bytesRead != Count) {
            return bytesRead;
        }

        for (i = 0; i < Count; i++) {
            value[i] = readBuff[i];
        }
    }
    return (Count);
}
