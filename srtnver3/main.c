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

#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"


d1type d1;
double ras[NSOU], decs[NSOU], epoc[NSOU];
int soutype[NSOU];
char sounam[NSOU][25];
char soutrack[25], souinfo[25];
GdkPixmap *pixmap = NULL;
GdkPixmap *vpixmap = NULL;
GdkFont *fixed_font;
GdkFont *vfixed_font;
GtkWidget *entry1, *entry2, *entry3, *entry5, *entry6, *entry8;
GtkWidget *table;
GtkWidget *vtable;
GtkWidget *vwindow = NULL;
GtkWidget *button_stow, *button_record, *button_cmdfl, *button_npoint, *button_bsw, *button_exit, *button_cal;
GtkWidget *drawing_area;
GtkWidget *vdrawing_area;
GtkTooltips *tooltips;
float avspec[NSPEC], aavspec[NSPEC];
float avspecon[NSPEC];
float avspecoff[NSPEC];
float bspec[NSPEC];
float bbspec[NSPEC];
float spec[NSPEC];
float fft1[NSPEC * 2];
float scanpwr[26];
double pwr;
double pwrst;
double pwrprev;
double pprev;
double polyb[NPOLY];
int midx, midy;



int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button_clear, *button_azel, *button_freq, *button_offset;
    GtkWidget *button_help;
    GdkColor color;
    int i, ii, mode;
    int yr, da, hr, mn, sc;
    double secstart;
    char buf[64];
    GdkGeometry geometry;
    GdkWindowHints geo_mask;
//    GdkRectangle update_rect;
    mode = 0;
    sprintf(d1.catnam, "srt.cat");
    sprintf(d1.hlpnam, "srt.hlp");
    for (i = 0; i < argc - 1; i++) {
        sscanf(argv[i], "%63s", buf);
        if (strstr(buf, "-c") && strlen(buf) == 2)
            sscanf(argv[i + 1], "%63s", d1.catnam);
        if (strstr(buf, "-h") && strlen(buf) == 2)
            sscanf(argv[i + 1], "%63s", d1.hlpnam);
        if (strstr(buf, "-m") && strlen(buf) == 2)
            sscanf(argv[i + 1], "%d", &mode);
    }
//    d1.azelport = 0x3f8;        // com1 default for old SRT 
    d1.secs = readclock();
    d1.run = 1;
    d1.record = 0;
    d1.entry1 = d1.entry2 = d1.entry3 = d1.entry5 = d1.entry6 = d1.entry8 = d1.helpwindow = d1.vwindow = 0;
    d1.plot = 0;
    d1.start_time = 0.0;
    d1.speed_up = 0;
    d1.ppos = 0;
    d1.printout = 1;
    d1.debug = 0;
    d1.freq = 1420.4;           // default
    d1.bw = 0;                  // set to 2.4 for TV dongle 10 MHz for ADC card in init
    d1.fbw = 0;                 // set in init or srt.cat
    d1.nblk = 5;                // number of blocks in vspectra
    d1.record_int_sec = 0;
    d1.freqcorr = 0;            // frequency correction for L.O. may be needed for TV dongle
    d1.freqchng = 0;
    d1.clearint = 0;
    d1.nfreq = NSPEC;
    d1.plotsec = 1;
    d1.displ = 1;
    d1.noisecal = 0;
//    used for old SRT mount and controller
//    d1.ptoler = 1;
//    d1.countperstep = 10000;    // default large number for no stepping 
//    d1.elcounts_per_deg = (52.0 * 27.0 / 120.0); // default for H-180
//    d1.azcounts_per_deg = 8.0 * 32.0 * 60.0 / (360.0 * 9.0); // default for CASSIMOUNT
//    d1.rod = 1;                 // default to rod as on CASSIMOUNT
//    d1.rod1 = 14.25;            // rigid arm length
//    d1.rod2 = 16.5;             // distance from pushrod upper joint to el axis
//    d1.rod3 = 2.0;              // pushrod collar offset
//    d1.rod4 = 110.0;            // angle at horizon
//    d1.rod5 = 30.0;             // pushrod counts per inch
    d1.azelsim = d1.radiosim = d1.fftsim = 0;
    d1.mainten = 0;
    d1.calcons = 1.0;
    d1.caldone = 0;
    d1.nrfi = 0;
    d1.rfisigma = 6;            // level for RFI reporting to screen
    d1.tload = 300.0;
    d1.tspill = 20.0;
    d1.beamw = 5.0;
    d1.comerr = 0;
    d1.limiterr = 0;
    d1.restfreq = 1420.406;     /* H-line restfreq */
    d1.delay = 0;
    d1.azoff = 0.0;
    d1.eloff = 0.0;
    d1.drift = 0;
    d1.tstart = 0;
    d1.tsys = 100.0;            // expected on cold sky
    d1.pwroff = 0.0;
    d1.tant = 100.0;
    d1.calpwr = 0;
    d1.yfac = 0;
    d1.calon = 0;
    d1.calmode = 0;
    d1.docal = 0;
    d1.tcal = 290;              // absorber or bushes
    d1.sourn = 0;
    d1.track = 0;
    d1.scan = 0;
    d1.bsw = 0;
    d1.nbsw = 1;
    d1.obsn = 0;
    d1.stopproc = 0;
    d1.fstatus = 0;
    d1.cmdfl = 0;
    d1.south = 1;
    d1.hgt = 0;
    d1.dongle = 0;              // set to zero initially - set to 1 in Init_Device if dongle
    d1.npoly = 25;              // number of terms in polynomial fit of bandpass
    pwrst = pwrprev = 0.0;
    soutrack[0] = 0;
    sprintf(d1.cmdfnam, "cmd.txt");
    sprintf(d1.datadir, "./");  // default to local directory

    if (!catfile())
        return 0;
    d1.foutstatus = 0;
// to get permission su root chown root srtn then chmod u+s srtn then exit 
    if (!d1.azelsim) {
        if (d1.printout)
            printf("initializing antenna controller\n");
        i = rot2(&d1.aznow, &d1.elnow, -1, buf); // initialize
        i = rot2(&d1.aznow, &d1.elnow, 1, buf); // read
        if (i < 0) {
            printf("Couldn't talk to antenna controller\n");
            return 0;
        }
    } else {
        d1.azprev = d1.azlim1;
        d1.elprev = d1.ellim1;
    }
    setgid(getgid());
    setuid(getuid());
    if (d1.mainten == 0) {
        d1.azcmd = d1.azlim1;
        d1.elcmd = d1.ellim1;
        d1.azcount = 0;
        d1.elcount = 0;
        d1.stow = 1;
    }
    if (d1.azlim1 > d1.azlim2) {
        d1.south = 0;           // dish pointing North for southern hemisphere
        if (d1.azlim2 < 360.0)
            d1.azlim2 += 360.0;
    }

    if (!d1.radiosim)
        Init_Device(0);

    if (d1.displ) {
        gtk_init(&argc, &argv);
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        geometry.min_width = 500;
        geometry.min_height = 300;
        geo_mask = GDK_HINT_MIN_SIZE;
        gtk_window_set_geometry_hints(GTK_WINDOW(window), window, &geometry, geo_mask);
        //Table size determines number of buttons across the top
        table = gtk_table_new(30, NUMBUTTONS, TRUE);

        drawing_area = gtk_drawing_area_new();
        gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
        color.red = 0xffff;
        color.blue = 0xffff;
        color.green = 0xffff;
        gtk_widget_show(drawing_area);
        gtk_table_attach_defaults(GTK_TABLE(table), drawing_area, 0, NUMBUTTONS, 3, 30);

        g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(quit), NULL);

        gtk_container_add(GTK_CONTAINER(window), table);

        g_signal_connect(G_OBJECT(drawing_area), "expose_event", (GtkSignalFunc) expose_event, NULL);
        g_signal_connect(G_OBJECT(drawing_area), "configure_event", (GtkSignalFunc) configure_event, NULL);

        g_signal_connect(G_OBJECT(drawing_area), "button_press_event",
                         (GtkSignalFunc) button_press_event, NULL);

        gtk_widget_set_events(drawing_area, GDK_EXPOSURE_MASK
                              | GDK_LEAVE_NOTIFY_MASK
                              | GDK_BUTTON_PRESS_MASK
                              | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);


        button_clear = gtk_button_new_with_label("clear");
        button_stow = gtk_button_new_with_label("stow");
        button_azel = gtk_button_new_with_label("azel");
        button_npoint = gtk_button_new_with_label("npoint");
        button_bsw = gtk_button_new_with_label("beamsw");
        button_freq = gtk_button_new_with_label("freq");
        button_offset = gtk_button_new_with_label("offset");
        button_record = gtk_button_new_with_label("record");
        button_cmdfl = gtk_button_new_with_label("cmdfl");
        button_cal = gtk_button_new_with_label("cal");
        button_help = gtk_button_new_with_label("help");
        button_exit = gtk_button_new_with_label("exit");

        g_signal_connect(G_OBJECT(button_clear), "clicked", G_CALLBACK(button_clear_clicked), NULL);
        g_signal_connect(G_OBJECT(button_stow), "clicked", G_CALLBACK(button_stow_clicked), NULL);
        g_signal_connect(G_OBJECT(button_azel), "clicked", G_CALLBACK(button_azel_clicked), NULL);
        g_signal_connect(G_OBJECT(button_npoint), "clicked", G_CALLBACK(button_npoint_clicked), NULL);
        g_signal_connect(G_OBJECT(button_bsw), "clicked", G_CALLBACK(button_bsw_clicked), NULL);
        g_signal_connect(G_OBJECT(button_freq), "clicked", G_CALLBACK(button_freq_clicked), NULL);
        g_signal_connect(G_OBJECT(button_offset), "clicked", G_CALLBACK(button_offset_clicked), NULL);
        g_signal_connect(G_OBJECT(button_record), "clicked", G_CALLBACK(button_record_clicked), NULL);
        g_signal_connect(G_OBJECT(button_cmdfl), "clicked", G_CALLBACK(button_cmdfl_clicked), NULL);
        g_signal_connect(G_OBJECT(button_cal), "clicked", G_CALLBACK(button_cal_clicked), NULL);
        g_signal_connect(G_OBJECT(button_help), "clicked", G_CALLBACK(button_help_clicked), NULL);
        g_signal_connect(G_OBJECT(button_exit), "clicked", G_CALLBACK(button_exit_clicked), NULL);

        // test setting up tooltips instead of the "enter"/"leave" used below
        tooltips = gtk_tooltips_new();
        gtk_tooltips_set_tip(tooltips, button_clear,
                             "click to clear integration and reset time plot to 1/4-scale", NULL);
        gtk_tooltips_set_tip(tooltips, button_stow, "click to stow antenna", NULL);
        gtk_tooltips_set_tip(tooltips, button_azel, "click to enter az el coordinates", NULL);
        gtk_tooltips_set_tip(tooltips, button_npoint, "click to start npoint scan", NULL);
        gtk_tooltips_set_tip(tooltips, button_bsw, "click to start beam switch", NULL);
        gtk_tooltips_set_tip(tooltips, button_freq, "click to enter new frequency [bw] [nfreq]", NULL);
        gtk_tooltips_set_tip(tooltips, button_offset, "click to enter offsets", NULL);
        if (!d1.cmdfl)
            gtk_tooltips_set_tip(tooltips, button_cmdfl, "click to start cmd file", NULL);
        else
            gtk_tooltips_set_tip(tooltips, button_cmdfl, "click to stop cmd file", NULL);

        gtk_tooltips_set_tip(tooltips, button_cal, "click to start calibration", NULL);
        gtk_tooltips_set_tip(tooltips, button_help, "click to open help window", NULL);
        record_tooltip();


        gtk_table_attach(GTK_TABLE(table), button_clear, 0, 1, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_stow, 1, 2, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_azel, 2, 3, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_npoint, 3, 4, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_bsw, 4, 5, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_freq, 5, 6, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_offset, 6, 7, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_record, 7, 8, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_cmdfl, 8, 9, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_cal, 9, 10, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_help, 10, 11, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
        gtk_table_attach(GTK_TABLE(table), button_exit, 11, 12, 0, 2, GTK_FILL, GTK_FILL, 0, 0);



        gtk_widget_show(button_clear);
        gtk_widget_show(button_stow);
        gtk_widget_show(button_azel);
        gtk_widget_show(button_npoint);
        gtk_widget_show(button_bsw);
        gtk_widget_show(button_freq);
        gtk_widget_show(button_offset);
        gtk_widget_show(button_record);
        gtk_widget_show(button_cmdfl);
        gtk_widget_show(button_cal);
        gtk_widget_show(button_help);
        gtk_widget_show(button_exit);


        gtk_widget_show(table);
        gtk_widget_show(window);
        clearpaint();
    }
    ii = 0;
    if (d1.printout) {
        toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
        printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
    }
    zerospectra(0);
    for (i = 0; i < d1.nfreq; i++)
        bspec[i] = 1;
    secstart = d1.nsecstart = -1;
    d1.secs = readclock();
    while (d1.run) {
        zerospectra(1);
        if (d1.clearint) {
            if (d1.displ)
                cleararea();
            zerospectra(0);
            d1.clearint = 0;
        }
        if (d1.freqchng) {
            if (d1.dongle)
                Init_Device(1);
            if (d1.printout) {
                toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
                printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
            }
            if (!d1.radiosim) {
                sleep(1);
            }
            zerospectra(0);
            d1.freqchng = 0;
        }
        if (d1.docal) {
            if (d1.bsw) {
                d1.bsw = 0;
                d1.azoff = 0.0;
            }
            if (d1.scan) {
                d1.scan = 0;
                d1.eloff = d1.azoff = 0.0;
            }
            if (d1.slew)
                d1.slew = 0;
            if (d1.docal == 1)
                cal(0);
            d1.docal = 2;
            cal(1);
            if (d1.integ >= NCAL) {
                cal(2);
                d1.docal = 0;
            }
        }

        if (d1.displ)
            cleararea();
        azel(d1.azcmd, d1.elcmd); // allow time after cal 
        if (!d1.slew) {
            pwr = 0.0;
        }
        if (!d1.slew)
            vspectra();
        d1.secs = readclock();
        aver();
        d1.integ2++;
        if (d1.record_int_sec && d1.integ2 >= d1.record_int_sec) {
            outfile();
            d1.integ2 = 0;
        }
        if (d1.displ) {
            if (!d1.plot)
                Repaint();
            while (gtk_events_pending() || d1.stopproc == 1) {
                gtk_main_iteration();
                d1.plot = 0;
            }
        }
        if (!d1.displ && d1.domap)
            scanplot();
    }
    return 0;
}


void zerospectra(int mode)
{
    int i, j, yr, da, hr, mn, sc;
    double az, el, secs, ra, dec;
    secs = d1.secs;

    if (!mode) {
        for (i = 0; i < d1.nfreq; i++)
            avspec[i] = avspecoff[i] = avspecon[i] = 0;
        d1.pwron = d1.pwroff = 0;
        d1.numon = d1.numoff = d1.integ = 0;
    }

    if (d1.cmdfl && secs > d1.secstop && !d1.slew && !d1.scan && !d1.docal && mode)
        d1.secstop = cmdfile();

    d1.vlsr = 0.0;
    az = -1;
    for (i = 0; d1.track >= 0 && i < d1.nsou; i++) {
        if (strstr(sounam[i], soutrack) && soutrack[0]) {
            toyrday(secs, &yr, &da, &hr, &mn, &sc);
            d1.year = yr;
            if (strstr(sounam[i], "Sun") || strstr(sounam[i], "Moon")) {
                if (strstr(sounam[i], "Sun"))
                    sunradec(secs, &ra, &dec);
                else
                    moonradec(secs, &ra, &dec);
                radec_azel(gst(secs) - ra - d1.lon, dec, d1.lat, &az, &el);
            } else if (soutype[i]) {
                az = ras[i] * PI / 180.0;
                el = decs[i] * PI / 180.0;
                azel_to_radec(secs, ras[i], decs[i], &ra, &dec);
            } else {
                precess(ras[i], decs[i], &ra, &dec, epoc[i], d1.year);
                radec_azel(gst(secs) - ra - d1.lon, dec, d1.lat, &az, &el);
            }
            d1.vlsr = vlsr(secs, ra, dec);
            sprintf(souinfo, "%s %4d", to_radecp(ra, dec), yr);
        }
    }
    if (d1.track && az >= 0.0) {
        if (d1.scan > 0) {
            i = (d1.scan - 1) / 5;
            j = (d1.scan - 1) % 5;
            d1.eloff = (i - 2) * d1.beamw * 0.5;
            d1.azoff = (j - 2) * d1.beamw * 0.5 / cos(el + d1.eloff * PI / 180.0);
            d1.scan++;
            if (d1.scan > 26) {
                d1.scan = 0;
                if (d1.displ)
                    gtk_tooltips_set_tip(tooltips, button_npoint, "click to start npoint scan", NULL);
                d1.azoff = d1.eloff = 0;
                d1.domap = 1;
            }
        }
        if (d1.bsw == 1)
            d1.bswint = 0;
        if (d1.bsw > 0 && d1.bswint == 0) {
            if (d1.bsw == 1)
                d1.clearint = 1;
            i = (d1.bsw - 1) % 4;
            j = 0;
            if (i == 1)
                j = -1;
            if (i == 3)
                j = 1;
            d1.azoff = j * d1.beamw / cos(el);
            d1.bsw++;
        }
    }
    if (az >= 0 && d1.stow != 1) {
        d1.azcmd = az * 180.0 / PI + d1.azoff;
        d1.elcmd = el * 180.0 / PI + d1.eloff;
//     printf("inzero azcmd %f ellim2 %f\n",d1.azcmd,d1.ellim2);
    } else {
        azel_to_radec(secs, d1.azcmd, d1.elcmd, &ra, &dec);
        d1.vlsr = vlsr(secs, ra, dec);
    }
    if (mode == 0) {
        d1.integ = 0.0;
        pwr = 0.0;
    }
}

void aver(void)
{
    int i, j, j1, j2;
    double p, a;
    GdkColor color;
    p = a = 0;
    j1 = d1.f1 * d1.nfreq;
    j2 = d1.f2 * d1.nfreq;
    for (i = 0; i < d1.nfreq; i++) {
        avspec[i] += spec[i];
        if (i > j1 && i < j2) {
            p += spec[i];
            a++;
        }
    }
    d1.integ++;
    i = d1.fc * d1.nfreq;
    if (spec[i] == 0.0)
        return;
    if (d1.calpwr == 0 && spec[i] > 0.0) {
//        printf("calpwr\n");
        for (j = 0; j < d1.nfreq; j++)
            bspec[j] = 1;
        if (d1.caldone && d1.displ) {
            d1.caldone = 0;
            color.green = 0xffff;
            color.red = 0xffff;
            color.blue = 0xffff;
            gtk_widget_modify_bg(button_cal, GTK_STATE_NORMAL, &color);
        }
        d1.calpwr = p / a;
    }
    pwr = (d1.tsys + d1.tcal) * p / (a * d1.calpwr);
    if (d1.caldone)
        d1.tant = pwr - d1.tsys;
    if (d1.calon) {
//      pwr=pwr*3.0;
        if (d1.yfac == 0.0)
            d1.yfac = pwr / pwrprev;
    }
//    printf("pwr %f pwrprev %f calon %d caldone %d yfac %f\n",pwr,pwrprev,d1.calon,d1.caldone,d1.yfac);
    if (d1.calon == 0)
        pwrprev = pwr;
    for (i = 0; i < d1.nfreq; i++)
        aavspec[i] = (d1.tsys + d1.tcal) * avspec[i] / (bspec[i] * d1.calpwr * d1.integ);
//  printf("d1.calpwr %f %f\n",d1.calpwr, aavspec[(int)(0.4*d1.nfreq)]);
    if (d1.scan != 0) {
        scanpwr[d1.scan - 1] = pwr;
    }
    if (d1.bsw && d1.track) {

        j = (d1.bsw - 1) % 4;
        if (j == 1 || j == 3) {
            for (i = 0; i < d1.nfreq; i++)
                avspecon[i] += spec[i];
            d1.pwron += pwr;
            d1.numon++;
        } else {
            for (i = 0; i < d1.nfreq; i++)
                avspecoff[i] += spec[i];
            d1.pwroff += pwr;
            d1.numoff++;
        }
        if (d1.numon && d1.numoff) {
            for (i = 0; i < d1.nfreq; i++)
                aavspec[i] =
                    d1.tsys * (avspecon[i] / d1.numon -
                               avspecoff[i] / d1.numoff) / ((avspecon[i] + avspecoff[i]) / (d1.numon +
                                                                                            d1.numoff));
            d1.bswpwr =
                d1.tsys * (d1.pwron / d1.numon -
                           d1.pwroff / d1.numoff) / ((d1.pwron + d1.pwroff) / (d1.numon + d1.numoff));
// printf("j %d d1.azoff %f pwr %f\n",j,d1.azoff,pwr);
        }
        d1.bswint++;
        if (d1.bswint >= d1.nbsw)
            d1.bswint = 0;
    }

}

double gauss(void)
{
    double v1, v2, r, fac, aamp, vv1;
    static int j;
    v1 = r = 0.0;
    while (r > 1.0 || r == 0.0) {
        v1 = 2.0 * (rand() / 2147483648.0) - 1.0;
        v2 = 2.0 * (rand() / 2147483648.0) - 1.0;
        r = v1 * v1 + v2 * v2;
    }
    fac = sqrt(-2.0 * log(r) / r);
    vv1 = v1 * fac;
    aamp = vv1;
    j = 1;
    return (aamp);
}
