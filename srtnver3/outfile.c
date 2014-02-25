#include <stdio.h>
#include <math.h>
#include <string.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"


void outfile(void)
{
    int yr, da, hr, mn, sc, i, istart, istop;
    float freqsep, sigma;
    GdkColor color;
    static FILE *file1 = NULL;
    static int n;
    if (!d1.record)
        return;
    if (d1.foutstatus == 0)     // set filename
    {
        n = 0;
        toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
        d1.rday = da;
        if (d1.filname[0] == 0)
            sprintf(d1.filname, "%s%4d_%03d_%02d.rad", d1.datadir, yr, da, hr);
        if (d1.displ) {
            color.green = 0xffff;
            color.red = 0;
            color.blue = 0;
            gdk_color_parse("green", &color);
            gtk_widget_modify_bg(button_record, GTK_STATE_NORMAL, &color);
        }
    }
    if ((file1 = fopen(d1.filname, "a")) == NULL) {
        if ((file1 = fopen(d1.filname, "w")) == NULL) {
            d1.foutstatus = -99;
            printf("cannot write %s\n", d1.filname);
            return;
        }
        d1.obsn = 0;            // file was new so reset obsn
        d1.foutstatus = 1;
    } else
        d1.foutstatus = 1;
    toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
    if (d1.foutstatus == 1) {
        if (d1.newday && da != d1.rday) {
            fclose(file1);
            d1.foutstatus = 0;
            toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
            sprintf(d1.filname, "%s%4d_%03d_%02d.rad", d1.datadir, yr, da, hr);
            if ((file1 = fopen(d1.filname, "w")) == NULL) {
                d1.foutstatus = -99;
                return;
            }
            d1.foutstatus = 1;
        }
        if (d1.printout) {
            toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
            printf("%4d:%03d:%02d:%02d:%02d %3s file: %s pwr %f\n", yr, da, hr, mn, sc, d1.timsource,
                   d1.filname, pwr);
        }
    }
    freqsep = d1.bw / d1.nfreq;
    if (soutrack[0] > 0)
        fprintf(file1,
                "DATE %4d:%03d:%02d:%02d:%02d obsn %3d az %3.0f el %2.0f freq_MHz %10.4f Tsys %6.3f Tant %6.3f vlsr %7.2f glat %6.3f glon %6.3f source %s\n",
                yr, da, hr, mn, sc, d1.obsn, d1.aznow, d1.elnow, d1.freq, d1.tsys, d1.tant, d1.vlsr, d1.glat,
                d1.glon, soutrack);
    else
        fprintf(file1,
                "DATE %4d:%03d:%02d:%02d:%02d obsn %3d az %3.0f el %2.0f freq_MHz %10.4f Tsys %6.3f Tant %6.3f vlsr %7.2f glat %6.3f glon %6.3f source at_stow\n",
                yr, da, hr, mn, sc, d1.obsn, d1.aznow, d1.elnow, d1.freq, d1.tsys, d1.tant, d1.vlsr, d1.glat,
                d1.glon);
    if (d1.record_spec) {
        istart = d1.f1 * d1.nfreq + 0.5;
        istop = d1.f2 * d1.nfreq + 0.5;
        sigma = d1.tsys / sqrt((d1.nsam * d1.integ / (2.0e6 * d1.bw)) * freqsep * 1e6);
        if (d1.bsw)
            sigma = 2.0 * sigma;
        fprintf(file1,
                "Fstart %8.3f fstop %8.3f spacing %8.6f bw %8.3f fbw %8.3f MHz nfreq %d nsam %d npoint %d integ %5.0f sigma %8.3f bsw %d\n",
                istart * d1.bw / d1.nfreq + d1.efflofreq, istop * d1.bw / d1.nfreq + d1.efflofreq, freqsep,
                d1.bw, d1.fbw, d1.nfreq, d1.nsam, istop - istart, d1.integ * d1.nsam / (2.0e6 * d1.bw), sigma,
                d1.bsw);
        fprintf(file1, "Spectrum \n");
        for (i = istart; i < istop; i++)
            fprintf(file1, "%8.3f ", aavspec[i]);
        fprintf(file1, "\n");
    }
    d1.obsn++;
    if (d1.foutstatus == 1)
        fclose(file1);
    d1.rday = da;
}
