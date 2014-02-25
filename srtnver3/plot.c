#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"


gint Repaint(void)
{
    GdkRectangle update_rect;
    GdkColor color;
    char txt[80], *tx2;
    int i, j, x, y, yp, yr, da, hr, mn, sc, ix, iy, istart, istop;
    int x1, y1, x2, y2, xst, yst, iav;
    double secs, lst, ra, dec, az, el, c, max, min, xsz, ddt, azz1, azz2;;

    midx = drawing_area->allocation.width / 2;
    midy = drawing_area->allocation.height / 2;
    if (d1.south) {
        azz1 = d1.azlim1;
        azz2 = d1.azlim2;
    } else {
        azz1 = d1.azlim1 - 180.0;
        azz2 = d1.azlim2 - 180.0;
    }
    xst = yst = 0;
    ddt = 0;
    xsz = 0.18 * 2;
    gdk_draw_line(pixmap, drawing_area->style->black_gc, 0, midy * 2, midx * 2, midy * 2);
    gdk_draw_line(pixmap, drawing_area->style->black_gc, 0, midy, midx * 2, midy);
    gdk_draw_line(pixmap, drawing_area->style->black_gc, 0, midy, 0, midy * 2);
    gdk_draw_line(pixmap, drawing_area->style->black_gc, midx * 2, midy, midx * 2, midy * 2);
    for (i = 10; i < 360; i += 10) {
        x = i * midx / 180;
        gdk_draw_line(pixmap, drawing_area->style->black_gc, x, midy * 2 - 2, x, midy * 2);
        if (i % 30 == 0) {
            if (d1.south)
                sprintf(txt, "%03d", i);
            else {
                if (i >= 180)
                    sprintf(txt, "%03d", i - 180);
                else
                    sprintf(txt, "%03d", i + 180);
            }
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc,
                          x - 8, midy * 2 - 8, txt, strlen(txt));
        }
    }
    for (i = 10; i < 90; i += 10) {
        y = midy * 2 - i * midy / 90;
        gdk_draw_line(pixmap, drawing_area->style->black_gc, 0, y, 2, y);
        if (i % 30 == 0) {
            sprintf(txt, "%02d", i);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, 5, y + 5, txt, strlen(txt));
        }
    }
    x2 = y2 = 0;
    j = 0;
    for (i = d1.ellim1; i <= 90; i += 1) {
        x = azz1 * midx / 180;
        y = midy * 2 - i * midy / 90;
        if (i <= d1.ellim2 && j % 2)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x, y2, x, y);
        x = azz2 * midx / 180;
        if (i <= d1.ellim2 && j % 2)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x, y2, x, y);
        y2 = y;
        j++;
    }
    j = 0;
    for (i = azz1; i <= azz2; i += 1) {
        x = i * midx / 180;
        y = midy * 2 - d1.ellim1 * midy / 90;
        if (j % 2)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x2, y, x, y);
        y = midy * 2 - d1.ellim2 * midy / 90;
        if (d1.ellim2 <= 90.0 && j % 2)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x2, y, x, y);
        x2 = x;
        j++;
    }
    j = 0;
    for (i = -d1.ellim2 + 180.0; i <= 90; i += 1) {
        x = (azz1 + 180.0) * midx / 180;
        y = midy * 2 - i * midy / 90;
        if (j % 2)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x, y2, x, y);
        x = (azz2 - 180.0) * midx / 180;
        if (j % 2)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x, y2, x, y);
        y2 = y;
        j++;
    }
    j = 0;
    for (i = azz1 + 180.0; i <= 360; i += 1) {
        x = i * midx / 180;
        y = midy * 2 - (180.0 - d1.ellim2) * midy / 90;
        if (d1.ellim2 > 90.0 && j % 2)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x2, y, x, y);
        x2 = x;
        j++;
    }
    for (i = 0; i <= azz2 - 180.0; i += 1) {
        x = i * midx / 180;
        y = midy * 2 - (180.0 - d1.ellim2) * midy / 90;
        if (d1.ellim2 > 90.0 && i % 2)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x2, y, x, y);
        x2 = x;
    }
    secs = d1.secs;
    sprintf(txt, "secs %f", secs);
    toyrday(secs, &yr, &da, &hr, &mn, &sc);
    d1.year = yr;
    sprintf(txt, "%4d:%03d:%02d:%02d:%02d %3s", yr, da, hr, mn, sc, d1.timsource);
    ix = midx * 1.55;
    iy = midy * 0.35;
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    tx2 = to_date(yr, da);
    iy = midy * 0.40;
    lst = gst(secs) - d1.lon;
    if (lst > PI * 2.0)
        lst -= PI * 2.0;
    if (lst < 0.0)
        lst += PI * 2.0;
    sprintf(txt, "%s LST %5.2f h", tx2, lst * 12.0 / PI);
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));

    sunradec(secs, &ra, &dec);
    radec_azel(gst(secs) - ra - d1.lon, dec, d1.lat, &az, &el);
    if (d1.debug)
        printf("Sun Az %f El %f south %d midx %d midy %d\n", az * 180.0 / PI, el * 180.0 / PI, d1.south, midx,
               midy);
    /* plot sources */
    for (i = 0; i < d1.nsou; i++) {
        if (strstr(sounam[i], "Sun") || strstr(sounam[i], "Moon")) {
            if (strstr(sounam[i], "Sun"))
                sunradec(secs, &ra, &dec);
            else
                moonradec(secs, &ra, &dec);
            radec_azel(gst(secs) - ra - d1.lon, dec, d1.lat, &az, &el);
        } else if (soutype[i]) {
            az = ras[i] * PI / 180.0;
            el = decs[i] * PI / 180.0;
        } else {
            precess(ras[i], decs[i], &ra, &dec, epoc[i], d1.year);
            radec_azel(gst(secs) - ra - d1.lon, dec, d1.lat, &az, &el);
        }
        if (el > 0.0) {
            sprintf(txt, "%s", sounam[i]);
            if (d1.south)
                x = az * midx / PI;
            else {
                if (az >= 0 && az < PI)
                    x = midx + az * midx / PI;
                else
                    x = midx + (az - 2.0 * PI) * midx / PI;
            }
            y = midy * 2.0 - el * midy * 2 / PI;
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x - 2, y, x + 2, y);
            if(y > midy + 2) y1 = y - 2;
            else y1 = midy;
            gdk_draw_line(pixmap, drawing_area->style->black_gc, x, y1, x, y + 2);
            if(y < midy*1.05) y = midy*1.05;
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, x + 2, y, txt, strlen(txt));
        }
    }


    if (d1.azelsim) {
        sprintf(txt, "antenna motion simulated");
        iy = midy * 0.45;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    }
    sprintf(txt, "Source: %s", soutrack);
    iy = midy * 0.50;
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    sprintf(txt, "%s", souinfo);
    iy = midy * 0.55;
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    sprintf(txt, "Freq %8.3f MHz", d1.freq);
    iy = midy * 0.75;
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    sprintf(txt, "I.F. %8.3f MHz", d1.bw * 0.5);
    iy = midy * 0.80;
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    sprintf(txt, "BW   %8.3f MHz", d1.fbw);
    iy = midy * 0.85;
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));

    sprintf(txt, "RESOL%8.3f kHz", d1.bw * 1e3 / d1.nfreq);
    iy = midy * 0.90;
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));


    if (d1.south)
        x = d1.aznow * midx / 180.0;
    else {
        if (d1.aznow > 180.0)
            x = midx + (d1.aznow - 360.0) * midx / 180.0;
        else
            x = midx + d1.aznow * midx / 180.0;
    }
    y = midy * 2 - d1.elnow * midy * 2.0 / 180.0;
    color.red = 0xffff;
    color.green = color.blue = 0;
    gdk_color_parse("red", &color);

    gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);
    gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x - 4, y, x + 4, y);
    gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x, y - 4, x, y + 4);

//  gdk_draw_line(pixmap,drawing_area->style->black_gc,x-4,y,x+4,y);
    //  gdk_draw_line(pixmap,drawing_area->style->black_gc,x,y-4,x,y+4);
    /* plot Galactic plane */
    for (i = 0; i < 360; i += 10) {
        GalactictoRadec(0, i, &ra, &dec);
        radec_azel(gst(secs) - ra - d1.lon, dec, d1.lat, &az, &el);
        if (el > 0.0) {
            if (d1.south)
                x = az * midx / PI;
            else {
                if (az > PI)
                    x = midx + (az - 2.0 * PI) * midx / PI;
                else
                    x = midx + az * midx / PI;
            }
            y = midy * 2.0 - el * midy * 2.0 / PI;
            gdk_draw_point(pixmap, drawing_area->style->black_gc, x, y);
        }
// printf("i %d az %f el %f\n",i,az*180.0/PI,el*180.0/PI);
    }
/* plot spectra vs time and freq */
    sprintf(txt, "vlsr %3.0fkm/s l=%3.0f b=%3.0f", d1.vlsr, d1.glon, d1.glat);
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc,
                  midx * 1.55, midy * 0.6, txt, strlen(txt));
    sprintf(txt, "integ=%5.0f sec", d1.integ * d1.nsam / (2.0e6 * d1.bw));
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc,
                  midx * 1.55, midy * 0.65, txt, strlen(txt));
    if (d1.tsys > 0.0)
        sprintf(txt, "%3.1f-%3.1fMHz IF", d1.f1 * d1.bw, d1.f2 * d1.bw);
    else
        sprintf(txt, "uncalibrated");
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, midx * 1.2,
                  midy * 0.37, txt, strlen(txt));
    if (d1.radiosim) {
        sprintf(txt, "simulated");
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, midx * 1.2,
                      midy * 0.05, txt, strlen(txt));
    } else {
        sprintf(txt, "%3.1fMHz IF", d1.bw);
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, midx * 1.2,
                      midy * 0.05, txt, strlen(txt));
    }
    sprintf(txt, "av. spectrum");
    if (d1.bsw)
        sprintf(txt, "beamswitch %d", d1.bsw);
    gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, midx * 1.2,
                  midy * 0.69, txt, strlen(txt));
    for (iav = 0; iav < 3; iav++) {
        max = 1e-6;
        min = 1e99;
        istart = 0;
        istop = d1.nfreq;
        if (iav > 0) {
            istart = d1.nfreq * d1.f1;
            istop = d1.nfreq * d1.f2;
        }
        for (i = istart; i < istop; i++) {
            c = spec[i];
            if (iav == 1)
                c = spec[i] / bspec[i];
            if (iav == 2)
                c = aavspec[i];
            if (c > max)
                max = c;
            if (c < min)
                min = c;
        }
// printf("iav %d max %f min %f\n",iav,max,min);
        yst = midy * 0.0 + iav * midy * 0.32;
        xst = midx + midx * 0.1;
        x1 = xst;
        x2 = x1 + midx * xsz;
        y1 = yst;
        y2 = y1 + midy * 0.25;
        if (iav && pwr >= 0.0) {
            ddt = d1.tsys * (max - min) / max;
            if (d1.bsw)
                ddt = (max - min);
            if (d1.tsys > 0.0) {
                if (!d1.bsw || iav == 1) {
                    if (d1.caldone)
                        sprintf(txt, "fs %5.2fK Tant %4.1fK", ddt, d1.tant);
                    else
                        sprintf(txt, "fs %5.2fK pwr %4.1f", ddt, pwr);
                } else
                    sprintf(txt, "fs %5.2fK bswpwr %5.2f", ddt, d1.bswpwr);
            } else
                sprintf(txt, "%5.1e", ddt);
            gdk_draw_text(pixmap, fixed_font,
                          drawing_area->style->black_gc, x1, y2 + midy * 0.05, txt, strlen(txt));
        }

        gdk_draw_line(pixmap, drawing_area->style->black_gc, x1, y1, x2, y1);
        gdk_draw_line(pixmap, drawing_area->style->black_gc, x1, y2, x2, y2);
        gdk_draw_line(pixmap, drawing_area->style->black_gc, x1, y1, x1, y2);
        gdk_draw_line(pixmap, drawing_area->style->black_gc, x2, y1, x2, y2);

        if (d1.ppos > 399)
            d1.ppos = 0;
        if (iav < 2)
            min = 0;
        for (i = istart; i < istop; i++) {
            if (iav == 0)
                x2 = xst + i * midx * xsz / d1.nfreq;
            if (iav > 0)
                x2 = xst + (i - istart) * midx * xsz / (d1.nfreq * d1.fbw / d1.bw); // 2 to 6 MHz
            c = spec[i];
            if (iav == 1)
                c = spec[i] / bspec[i];
            if (iav == 2)
                c = aavspec[i];
            c = max * (c - min) / (max - min + 1e-99);
            if (c < 0.0)
                c = 0.0;
            if (c > max)
                c = max;
            y2 = yst + midy * 0.25 - c * midy * 0.25 / max + 1;

            gdk_draw_line(pixmap, drawing_area->style->black_gc, x1, y1, x2, y2);
            x1 = x2;
            y1 = y2;
        }
    }
    sprintf(txt, "%s", d1.filname);
    iy = midy * 0.95;
    if (d1.record) {
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc,
                      ix - midx * 0.1, iy, txt, strlen(txt));
        if (d1.foutstatus == 0) {
            iy = midy * 0.99;
            sprintf(txt, "record activated");
//            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));

        }
    }
    if (d1.tsys > 0.0) {
        iy = midy * 0.30;
        if (!d1.caldone)
            sprintf(txt, "Tsys %4.0f smax %4.0f", d1.tsys, d1.smax);
        else
            sprintf(txt, "Tsys %4.0f smax %4.0f", d1.tsys + d1.tant, d1.smax);
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, midx * 1.1, iy, txt, strlen(txt));
    }
    if (!d1.slew) {
        xsz = 1.0;
        if (pwr > 0.0 && (d1.ppos == 0 || pwrst == 0.0)) {
            pwrst = pwr;
            pprev = pwr;
        }
        x = d1.ppos * midx / 400;
        y = (1.0 - 0.25 * pwr / (pwrst + 1e-99)) * midy / xsz; // quarter scale
        yp = (1.0 - 0.25 * pprev / (pwrst + 1e-99)) * midy / xsz;
        if (y < 0)
            y = 0;
        if (y > midy / xsz)
            y = midy / xsz;
        if (yp < 0)
            yp = 0;
        if (yp > midy / xsz)
            yp = midy / xsz;
        if (d1.ppos > 0 && pwr > 0.0)
            gdk_draw_line(pixmap, drawing_area->style->black_gc, (int) (x - midx / 400), yp, x, y);
        if (pwr > 0.0 || d1.ppos == 0)
            pprev = pwr;
        if (x + 8 < drawing_area->allocation.width)
            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, x + 4, 0, 4, midy);
// printf("rect ppos %d\n",d1.ppos);
    }
    if (d1.ptick && fmod(secs, 60) < 2.5) {
        x = d1.ppos * midx / 400;
        gdk_draw_line(pixmap, drawing_area->style->black_gc, x, midy, x, midy - 5);
        d1.ptick = 0;
    }
    if (!d1.slew)
        d1.ppos++;
    if (fmod(secs, 60) >= 2.5)
        d1.ptick = 1;
    update_rect.x = update_rect.y = 0;
    update_rect.width = drawing_area->allocation.width;
    update_rect.height = drawing_area->allocation.height;
    if (d1.domap)
        scanplot();
//  printf("draw %d %d %d %d \n",update_rect.x,update_rect.y,update_rect.width,update_rect.height);
//    gtk_widget_draw(drawing_area, &update_rect);
//  gtk_widget_queue_draw_area(drawing_area, update_rect.x,update_rect.y,update_rect.width,update_rect.height);
    d1.plot = 1;
    return 1;
}
