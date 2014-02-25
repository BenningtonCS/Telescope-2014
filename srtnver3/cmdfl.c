#include <stdio.h>
#include <math.h>
#include <string.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"


double cmdfile(void)
  // reads command file
  /* drives the schedule with stop time yyyy:ddd:hh:mm:ss
     or LST:hh:mm:ss  or just : for immediate scheduling
     or :n for scheduling for n secs - will wait for source to be within limits
     followed by keywords:
     sourcename (any name in catalog)
     stow
     calibrate
     quit
     clearint      // clears intergation 
     record (turns on data file if not already on) [filename] [recmode]
     roff (turns off data file)
     freq fcenter_MHz [bandwidth_MHz]
     mode n for 25 point b for beamswitch
   */
{
    double secs, secnow, lst, freq, bw;
    int i, j, n, line, yr, day, hr, min, se, ss;
    int da, mn, sc;
    int ix, iy;
    char str[256], txt[256];
    char *p, *k;
    FILE *file1;
    GdkColor color;

    if ((file1 = fopen(d1.cmdfnam, "r")) == NULL) {
        printf(" Unable to open %s\n", d1.cmdfnam);
        return 0.0;
    }
    secs = secnow = d1.secs;
    i = 1;
    line = 0;
    ss = 0;
    k = 0;
    str[255] = 0;
    while ((k = fgets(str, 80, file1)) != 0 && i == 1) {
        line++;
// printf("line %d %s",line,str);
        if (str[0] != '*' && str[0] != ' ' && str[0] != '#' && strlen(str) > 2 && line > d1.cmdfline) {
            if (str[0] != 'L' && str[0] != ':') { // yyyy:ddd:hh:mm:ss
                sscanf(str, "%d:%d:%d:%d:%d", &yr, &day, &hr, &min, &se);
                secs = tosecs(yr, day, hr, min, se);
            }
//            d.dtext(440.0, ylim + 40.0, gg, Color.red, "cmd err " + str);
            if (str[0] == 'L' && strstr(str, "LST")) { //    "LST:%2d:%2d:%2d",&hr,&min,&sec
                hr = min = se = 0;
                sscanf(str, "LST:%d:%d:%d", &hr, &min, &se);
                lst = gst(secnow) - d1.lon;
                secs = hr * 3600.0 + min * 60.0 + se - lst * 86400.0 / (PI * 2.0);
                if (secs < 0)
                    secs += 86400.0;
                if (secs > 86400.0)
                    secs -= 86400.0;
                if (secs < 0)
                    secs += 86400.0;
                if (secs > 86400.0)
                    secs -= 86400.0;
                secs += secnow;
            }
//            d.dtext(440.0, ylim + 40.0, gg, Color.red, "cmd err " + str);
            if (str[0] == ':' && str[1] != ' ') {
                ss = 0;
                sscanf(str, ":%d", &ss);
                secs += ss;
            }
//            d.dtext(440.0, ylim + 40.0, gg, Color.red, "cmd err " + str);
            for (n = 0; n < (int) strlen(str); n++)
                if (str[n] == '\n')
                    str[n] = ' ';
            if (secs >= secnow) {
// System.out.println("secs "+secs+" secnow "+secnow);
                for (j = 0; j < d1.nsou; j++) {
                    if ((p = strstr(str, sounam[j]))) {
                        strncpy(soutrack, sounam[j], sizeof(soutrack) - 1);
                        if (ss < 2)
                            secs += 2; // add time
                        d1.track = 1;
                        d1.bsw = 0;
                        d1.azoff = 0;
                        d1.eloff = 0;
                        // check for mode
                        if (p && *p) { // note p before *p
                            p = strchr(p, ' ');
                            if (p && *p) {
                                if (strchr(p, 'n')) {
                                    d1.scan = 1;
                                }
                                if (strchr(p, 'b')) {
                                    d1.bsw = 1;
                                }
                            }

                        }
                    }
                }
                if (strstr(str, "azel")) {
                    j = sscanf(str, "%*s %*s %lf %lf", &d1.azcmd, &d1.elcmd);
                    if (j == 2) {
                        if (d1.printout) {
                            toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
                            printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
                            printf("cmdf %s", str);
                        }
                        if (ss < 2)
                            secs += 2; // add time
                        d1.track = 0;
                        soutrack[0] = 0;
                    }
                }
                if (strstr(str, "offset")) {
                    sscanf(str, "%*s %*s %lf %lf", &d1.azoff, &d1.eloff);
                }
                if (strstr(str, "stow")) {
                    d1.azcmd = d1.azlim1;
                    d1.elcmd = d1.ellim1;
                    soutrack[0] = 0;
                    d1.stow = 1;
                }
                if (strstr(str, "calibrate"))
                    d1.docal = 1;
                if (strstr(str, "record")) {
                    d1.record = 1;
                    if (sscanf(str, "%*s %*s %255s", d1.filname) == 1)
                        d1.foutstatus = 1;
                }
                if (strstr(str, "quit") && d1.stow == -1) {
                    d1.run = 0;
                    gtk_exit(0);
                }
                if (strstr(str, "roff")) {
                    d1.record = 0;
                    if (d1.displ) {
                        color.green = 0xffff;
                        color.red = 0xffff;
                        color.blue = 0xffff;
                        gtk_widget_modify_bg(button_record, GTK_STATE_NORMAL, &color);
                    }
                }
                if (strstr(str, "clearint"))
                    d1.clearint = 1;
                if (strstr(str, "freq")) {
                    bw = d1.bw;
                    sscanf(str, "%*s %*s %lf %lf", &freq, &bw);
                    if (bw > 0 && bw <= 10.0)
                        if (freq > 1200.0 && freq < 1800.0) {
                            d1.freq = freq;
                            d1.f1 = 0.5 - bw * 0.5;
                            d1.f2 = 0.5 + bw * 0.5;
                            d1.fc = (d1.f1 + d1.f2) * 0.5;
                            if (d1.printout) {
                                toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
                                printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
                                printf("new freq %f %f\n", d1.freq, d1.freq);
                            }
                            d1.freqchng = 1;
                        }
                }
            }
            i = 0;
            ix = midx * 1.05;
            iy = midy * 0.99;
            if (d1.displ && str[0]) {
                color.red = 0;
                color.green = 0xffff;
                color.blue = 0;
                gdk_color_parse("green", &color);

                gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                                   iy - midy * 0.04, midx * 0.4, midy * 0.05);
                sprintf(txt, "line %2d %s", line, str);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->fg_gc[GTK_STATE_NORMAL], ix,
                              iy, txt, strlen(txt) - 1);
            }
            if (d1.printout) {
                toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
                printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
                printf("line %2d %s\n", line, str);
            }
        }
    }
    d1.cmdfline = line;
    fclose(file1);
    if (k == 0) {
        if (d1.displ) {
            ix = midx * 1.05;
            iy = midy * 0.99;
            color.red = 0;
            color.green = 0xffff;
            color.blue = 0;
            gdk_color_parse("green", &color);
            gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);
            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                               iy - midy * 0.04, midx * 0.4, midy * 0.05);
            sprintf(txt, "line %2d : end_of_file", line);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->fg_gc[GTK_STATE_NORMAL], ix,
                          iy, txt, strlen(txt) - 1);
        }
        d1.cmdfl = 0;
        d1.cmdfline = 0;
    }
    return secs;
}
