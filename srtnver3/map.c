#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"

void scanplot(void)
{
    GdkColor color;
    GdkPoint points[6];
// contours the 25 point map
    GdkGC *rgb_gc;
    int i, j, k, x, y, mx, my, py[100], ppx = 0;
    int ix, iy, midxx, midyy;
    int yr, da, hr, mn, sc;
    double sum, az, el, a, b, mmax, max, min, dx, dy, azwid, elwid;
    double R, G, B, H, S, V, var_h, var_1, var_2, var_3, var_i, var_r, var_g, var_b;
    char txt[80];
    if (d1.displ) {
        midx = drawing_area->allocation.width / 2;
        midy = drawing_area->allocation.height / 2;
        gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, 0, 0, midx / 2, midy);
    }
    max = mmax = -1e99;
    min = 1e99;
    ppx = 0;
    for (i = 0; i < 25; i++) {
        if (scanpwr[i + 1] > max)
            max = scanpwr[i + 1];
        if (scanpwr[i + 1] < min)
            min = scanpwr[i + 1];
    }
//printf("max %f min %f\n",max,min);
    mx = my = 0;
    for (x = -50; x < 50; x++) {
        for (y = -50; y < 49; y++) {
            sum = 0.0;
            for (i = 0; i < 25; i++) {
                az = (double) ((i % 5) - 2);
                el = (double) (i / 5 - 2);
                dx = x * 0.05 - az;
                dy = y * 0.05 - el;
                a = dx * PI;
                b = dy * PI;
                if (a == 0.0)
                    a = 1.0;
                else
                    a = sin(a) / a;
                if (b == 0.0)
                    b = 1.0;
                else
                    b = sin(b) / b;
                sum += a * b * (scanpwr[i + 1] - min);
            }
            a = sum;
            if (a > mmax) {
                mmax = a;
                mx = x;
                my = y;
            }
            k = (int) (sum * 7.0 / (max - min));
            j = 0;
            if (k < 0)
                k = 0;
            if (k > 7)
                k = 7;
            if (k <= 2)
                j = k;
            if (k == 3)
                j = 4;
            if (k == 4)
                j = 9;
            if (k == 5)
                j = 10;
            if (k == 6)
                j = 12;
            if (k == 7)
                j = 15;
            if ((k != py[y + 50] || k != ppx) && x > -50 && y > -50)
                j = 15;
            py[y + 50] = k;
            ppx = k;

            H = j / 15.0;
            S = V = 1.0;
            if (j == 0)
                H = S = 0.0;
            V = 1.0;

            if (S == 0)         //HSV values = From 0 to 1
            {
                R = V * 0xffff; //RGB results = From 0 to 0xffff
                G = V * 0xffff;
                B = V * 0xffff;
            } else {
                var_h = H * 6;
                var_i = (int) (var_h);
                var_1 = V * (1 - S);
                var_2 = V * (1 - S * (var_h - var_i));
                var_3 = V * (1 - S * (1 - (var_h - var_i)));

                if (var_i == 0) {
                    var_r = V;
                    var_g = var_3;
                    var_b = var_1;
                } else if (var_i == 1) {
                    var_r = var_2;
                    var_g = V;
                    var_b = var_1;
                } else if (var_i == 2) {
                    var_r = var_1;
                    var_g = V;
                    var_b = var_3;
                } else if (var_i == 3) {
                    var_r = var_1;
                    var_g = var_2;
                    var_b = V;
                } else if (var_i == 4) {
                    var_r = var_3;
                    var_g = var_1;
                    var_b = V;
                } else {
                    var_r = V;
                    var_g = var_1;
                    var_b = var_2;
                }

                R = var_r * 0xffff; //RGB results 
                G = var_g * 0xffff;
                B = var_b * 0xffff;
            }
            if (d1.displ) {
                color.red = R;
                color.green = G;
                color.blue = B;
                rgb_gc = gdk_gc_new(pixmap);
                gdk_gc_set_rgb_fg_color(rgb_gc, &color); // fast
//      gtk_widget_modify_fg (drawing_area, GTK_STATE_NORMAL, &color);  // slow
//  printf("R %f G %f B %f\n",R,G,B);

                midyy = midy * 0.5 - 50;
                midxx = 50;
                points[0].x = x + midxx;
                points[0].y = midyy - y;
                points[1].x = x + midxx + 1;
                points[1].y = midyy - y;

//  gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL],
                gdk_draw_line(pixmap, rgb_gc, points[0].x, points[0].y, points[1].x, points[1].y);
                g_object_unref(G_OBJECT(rgb_gc));
            }
        }
    }
    azwid = elwid = 0.0;
    for (x = -50; x < 50; x++) {
        for (y = -50; y < 49; y++) {
            sum = 0.0;
            for (i = 0; i < 25; i++) {
                az = (double) ((i % 5) - 2);
                el = (double) (i / 5 - 2);
                dx = x * 0.05 - az;
                dy = y * 0.05 - el;
                a = dx * PI;
                b = dy * PI;
                if (a == 0.0)
                    a = 1.0;
                else
                    a = sin(a) / a;
                if (b == 0.0)
                    b = 1.0;
                else
                    b = sin(b) / b;
                sum += a * b * (scanpwr[i + 1] - min);
            }
            a = sum;
            if (a >= mmax * 0.5 && x == mx)
                elwid++;
            if (a >= mmax * 0.5 && y == my)
                azwid++;
        }
    }
    d1.scnazoff = d1.beamw * 0.5 * 0.05 * mx / cos(d1.elcmd * PI / 180.0);
    d1.scneloff = d1.beamw * 0.5 * 0.05 * my;
    if (d1.printout) {
        toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
        printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
        printf("Scan results:");
        printf(" max %e at %4.1f %4.1f ", mmax, d1.scnazoff, d1.scneloff);
        printf("azwid %3.1f elwid %3.1f deg\n", azwid * 0.025 * d1.beamw, elwid * 0.025 * d1.beamw);
    }
    if (d1.ppos < 150)
        d1.ppos = 150;

    if (d1.displ) {
        ix = midx * 0.02;
        sprintf(txt, "scan results:");
        iy = midy * 0.80;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        sprintf(txt, "azeloff %2.0f %2.0f", d1.scnazoff, d1.scneloff);
        iy = midy * 0.85;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        sprintf(txt, "azelwid %3.1f %3.1f", azwid * 0.025 * d1.beamw, elwid * 0.025 * d1.beamw);
        iy = midy * 0.90;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        if (d1.tsys > 0) {
            sprintf(txt, "ratio %4.2f", max / min);
        } else
            sprintf(txt, "power_ratio %5.2f", max / min);
        iy = midy * 0.95;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    }
    d1.domap = 0;

}
