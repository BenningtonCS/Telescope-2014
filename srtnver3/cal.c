#include <gtk/gtk.h>
#include <sys/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"


void cal(int mode)
  // command cal
{
    int k;
    int ix, iy, midxr, ixe;
    char txt[80];
    double av, max, maxf, p, a;
    float wtt[NSPEC];
    GdkColor color;

    if (mode == 0) {
        for (k = 0; k < d1.nfreq; k++)
            bbspec[k] = 0;
        zerospectra(0);
        d1.calon = 1;
        d1.integ3 = 0;
    }
    if (mode == 1) {
        for (k = 0; k < d1.nfreq; k++)
            bbspec[k] += spec[k];
        d1.integ3++;
    }
    if (mode == 2) {
        d1.calon = 0;
        max = -1e99;
        maxf = 0;
        for (k = 0; k < d1.nfreq; k++) {
            if (k > d1.f1 * d1.nfreq && k < d1.f2 * d1.nfreq)
                wtt[k] = 1;
            else
                wtt[k] = 0;
            if (spec[k] > max) {
                max = spec[k];
                maxf = k * d1.bw / d1.nfreq + d1.efflofreq;
            }
        }
        if (d1.npoly > NPOLY)
            d1.npoly = NPOLY;
        polyfitr(d1.npoly, d1.nfreq, bbspec, wtt, bspec);
//      for(k=0;k<d1.nfreq;k+=10) printf("k %d wtt %f bbspec %f\n",k,wtt[k],bbspec[k]);
        if (d1.npoly == 1)
            for (k = 0; k < d1.nfreq; k++)
                bspec[k] = bbspec[k];
        p = a = 0;
        for (k = 0; k < d1.nfreq; k++) {
            if (wtt[k]) {
                p += bspec[k];
                a++;
            }
        }
        av = p / a;
        for (k = 0; k < d1.nfreq; k++)
            bspec[k] = bspec[k] / av;
        d1.calpwr = p / (a * d1.integ3);
        if (d1.yfac > 1.5 && d1.calmode == 20)
            d1.tsys = (d1.tcal - d1.yfac * 3.0) / (d1.yfac - 1.0); // put in Tsys  assumes CMB of 3 K
        if (d1.calmode == 0)
            printf("yfac %f Tsys %f\n", d1.yfac, 290.0 / (d1.yfac - 1.0));
        if (d1.caldone == 0 && d1.calmode != 2 && d1.calmode != 3)
            printf("yfac %f tsys %f pwr %f pwrprev %f calmode %d max_pwr %8.3f MHz\n", d1.yfac, d1.tsys, pwr,
                   pwrprev, d1.calmode, maxf);
        d1.yfac = 0;            // for next cal
        d1.caldone = 1;
        if (d1.displ) {
            color.green = 0xffff;
            color.red = 0;
            color.blue = 0;
            gdk_color_parse("green", &color);
            gtk_widget_modify_bg(button_cal, GTK_STATE_NORMAL, &color);
        }

    }

    ix = midx * 1.55;
    ixe = midx * 0.4;
    midxr = midx * 2 - ix;

    sprintf(txt, "calibration in progress");
    iy = midy * 0.1;
    if (d1.displ && mode)
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    iy = midy * 0.1;
    if (d1.displ && !mode) {
        gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                           iy - midy * 0.04, midxr, midy * 0.05);
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    }
    if (d1.debug)
        printf("incal mode %d\n", mode);
    return;
}

void polyfitr(int npoly, int nfreq, float ddata[], float wtt[], float dataout[])
{
    int i, j, k, kk, m1, m2;
    double re, dd, freq;
    long double aarr[NPOLY * NPOLY], bbrr[NPOLY * NPOLY];
    static float mcalc[NSPEC * NPOLY];
    for (i = 0; i < nfreq; i++) {
        kk = i * npoly;
        freq = (double) (i - d1.fc * d1.nfreq) / ((double) (d1.fc * nfreq));
        for (j = 0; j < npoly; j++) {
            mcalc[kk] = pow(freq, (double) j);
            kk++;
        }
    }
    for (i = 0; i < npoly; i++) {
        re = 0.0;
        m1 = i;
        for (k = 0; k < nfreq; k++) {
            dd = log10(ddata[k]);
            if (wtt[k] > 0)
                re += mcalc[m1] * dd * wtt[k];
            m1 += npoly;
        }
        bbrr[i] = re;
        for (j = 0; j <= i; j++) {
            re = 0.0;
            m1 = i;
            m2 = j;
            for (k = 0; k < nfreq; k++) {
                if (wtt[k] > 0)
                    re += mcalc[m1] * mcalc[m2] * wtt[k];
                m1 += npoly;
                m2 += npoly;
            }
            k = j + (i * (i + 1)) / 2;

            aarr[k] = re;
        }
    }
    MatrixInvert(npoly, aarr, bbrr);

//        for (j = 0; j < npoly; j++) printf("poly %d %f\n",j,(double)bbrr[j]);
    for (i = 0; i < nfreq; i++) {
        re = 0.0;
        freq = (double) (i - d1.fc * d1.nfreq) / ((double) (d1.fc * nfreq));
        for (j = 0; j < npoly; j++) {
            re += bbrr[j] * pow(freq, (double) j);
        }
        dd = pow(10.0, re);
        dataout[i] = dd;
    }
}

void MatrixInvert(int nsiz, long double aarr[], long double bbrr[])
{
    int ic0, id, i, j, ij, ic, n;
    long double re, mag, sumr;
    long double ttrr[NPOLY * NPOLY];
    aarr[0] = 1.0L / aarr[0];
    bbrr[0] = bbrr[0] * aarr[0];

    /* inversion by bordering */
    for (n = 1; n <= nsiz - 1; n++) {
        ic0 = (n * (n + 1)) / 2;
        id = ic0 + n;
        for (i = 0; i < n; i++) {
            sumr = 0.0L;
            ij = (i * (i + 1)) / 2; /* A(I,0) */
            for (j = 0; j < n; j++) {
                ic = ic0 + j;
                sumr += aarr[ic] * aarr[ij];
                if (j < i)
                    ij++;
                if (j >= i)
                    ij += j + 1;
            }
            ttrr[i] = sumr;
        }
        sumr = 0.0L;
        for (i = 0; i < n; i++) {
            ic = ic0 + i;
            sumr += ttrr[i] * aarr[ic];
        }
        re = aarr[id] - sumr;
        mag = re * re;
        if (mag > 0.0L)
            aarr[id] = re / mag;

        else {
            aarr[id] = 0.0L;
            printf("ffre reached zero\n");
        }
        sumr = 0.0L;
        for (i = 0; i < n; i++) {
            ic = ic0 + i;
            sumr += aarr[ic] * bbrr[i];
        }
        bbrr[n] = aarr[id] * (bbrr[n] - sumr);
        for (i = 0; i < n; i++)
            bbrr[i] += -ttrr[i] * bbrr[n];
        for (i = 0; i < n; i++) {
            ic = ic0 + i;
            aarr[ic] = -aarr[id] * ttrr[i];
        }
        ij = 0;
        for (i = 0; i < n; i++) {
            for (j = 0; j <= i; j++) {
                ic = ic0 + j;
                aarr[ij] += -ttrr[i] * aarr[ic];
                ij++;
            }
        }
    }
}
