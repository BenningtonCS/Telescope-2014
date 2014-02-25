#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sched.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "hproto.h"
#include "d1glob.h"



GtkWidget *button_psw, *button_vexit;

void velspec()
{
    GdkColor color;
    GdkGeometry geometry;
    GdkWindowHints geo_mask;
    if (d1.debug)
        printf("enter velspec\n");
    if (d1.vwindow) {
        gtk_widget_destroy(vwindow);
        d1.vwindow = 0;
    }
    if (d1.debug)
        printf("enter velspec1\n");

    vwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    d1.vwindow = 1;
    gtk_window_set_title(GTK_WINDOW(vwindow), "Spectrum");
    geometry.min_width = 300;
    geometry.min_height = 300;
    geo_mask = GDK_HINT_MIN_SIZE;
    gtk_window_set_geometry_hints(GTK_WINDOW(vwindow), vwindow, &geometry, geo_mask);
    vtable = gtk_table_new(30, NUMBUTTONS, TRUE);
    vdrawing_area = gtk_drawing_area_new();
    gtk_window_set_default_size(GTK_WINDOW(vwindow), 600, 400);
    color.red = 0xffff;
    color.blue = 0xffff;
    color.green = 0xffff;
    gtk_widget_show(vdrawing_area);

    gtk_table_attach_defaults(GTK_TABLE(vtable), vdrawing_area, 0, NUMBUTTONS, 2, 30);


    g_signal_connect(G_OBJECT(vwindow), "destroy", G_CALLBACK(vquit), NULL);

    gtk_container_add(GTK_CONTAINER(vwindow), vtable);

    g_signal_connect(G_OBJECT(vdrawing_area), "expose_event", (GtkSignalFunc) vexpose_event, NULL);
    g_signal_connect(G_OBJECT(vdrawing_area), "configure_event", (GtkSignalFunc) vconfigure_event, NULL);


    gtk_widget_set_events(vdrawing_area, GDK_EXPOSURE_MASK
                          | GDK_LEAVE_NOTIFY_MASK
                          | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
    button_psw = gtk_button_new_with_label("click to write postscript file");
    button_vexit = gtk_button_new_with_label("exit");
    g_signal_connect(G_OBJECT(button_psw), "clicked", G_CALLBACK(button_psw_clicked), NULL);
    g_signal_connect_swapped(G_OBJECT(button_vexit), "clicked",
                             G_CALLBACK(gtk_widget_destroy), G_OBJECT(vwindow));
    gtk_table_attach(GTK_TABLE(vtable), button_psw, 0, NUMBUTTONS - 1, 0, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(vtable), button_vexit, NUMBUTTONS - 1, NUMBUTTONS, 0, 2, GTK_FILL, GTK_FILL, 0,
                     0);
    gtk_widget_show(button_psw);
    gtk_widget_show(button_vexit);

    gtk_widget_show(vtable);
    gtk_widget_show(vwindow);
    d1.psw = 0;
    vclearpaint();
    if (d1.debug)
        printf("leave velspec\n");
}

void button_psw_clicked(void)
{
    d1.psw = 1;
    if (d1.vwindow)
        vclearpaint();
    d1.psw = 0;
}

void vclearpaint(void)
{
    GdkRectangle update_rect;
    gdk_draw_rectangle(vpixmap, vdrawing_area->style->white_gc, TRUE, 0, 0,
//            vdrawing_area->style->black_gc, TRUE, 0, 0,
                       vdrawing_area->allocation.width, vdrawing_area->allocation.height);
    update_rect.x = 0;
    update_rect.y = 0;
    update_rect.width = vdrawing_area->allocation.width;
    update_rect.height = vdrawing_area->allocation.height;
    vplot();
    gtk_widget_draw(vdrawing_area, &update_rect);
}

gint vconfigure_event(GtkWidget * widget)
{
    GdkRectangle update_rect;
    int newp = 0;
    int sz = 140;
    char txt[80];
    if (d1.debug)
        printf("in vconfig\n");
    if (!vpixmap || d1.vwid != widget->allocation.width || d1.vwhgt != widget->allocation.height)
        newp = 1;
    if (vpixmap && newp)
        gdk_pixmap_unref(vpixmap);
    if (newp) {
        if (d1.debug)
            printf("in vconfig2\n");

        sz = (widget->allocation.width / 72) * 10; // font = 110 with wid = 800  - works best over range of size
        if (sz > 140)
            sz = 140;
        sprintf(txt, "-misc-fixed-medium-r-*-*-*-%d-*-*-*-*-iso8859-*", sz); // specified iso to avoid 16-bit encode
        vfixed_font = gdk_font_load(txt);

        vpixmap = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
// printf("vpix %d %d %d %d\n", widget->allocation.width,widget->allocation.height,
//                                  vdrawing_area->allocation.width, vdrawing_area->allocation.height);

        d1.vwid = widget->allocation.width;
        d1.vwhgt = widget->allocation.height;
        d1.psw = 0;
        vclearpaint();
        update_rect.x = 0;
        update_rect.y = 0;
        update_rect.width = vdrawing_area->allocation.width;
        update_rect.height = vdrawing_area->allocation.height;
        gtk_widget_draw(vdrawing_area, &update_rect);
    }
    return TRUE;
}

gint vexpose_event(GtkWidget * widget, GdkEventExpose * event)
{
    if (d1.debug)
        printf("in vexpose\n");
    gdk_draw_pixmap(widget->window,
                    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                    vpixmap,
                    event->area.x, event->area.y,
                    event->area.x, event->area.y, event->area.width, event->area.height);
    //  printf("draw pixmap\n");   // the above actually draws to screen
    return FALSE;
}

void vquit()
{
    d1.vwindow = 0;
}

void vplot(void)
  // plot the spectrum
{
    char txt[80], fnam[80];
    int i, j, jmax, k, np, j1, j2, n3, npoint, yr, da, hr, mn, sc;
    double xx, yy, dmax, ddmax, dmin, slope, dd, ddd, totpp, scale, sigma, freq, fstart, fstop, vstart, vstop,
        xoffset;
    double freqsep, x1, x2, y1, y2, wid, sx, sy, yoffset, x, y, xp, yp, av, avx, avy, avxx, avxy, psx1, psx2,
        psy1, psy2, yps;
    double pp[NSPEC];
    FILE *file1 = NULL;
    if (d1.integ == 0.0)
        return;
    if (d1.psw) {
        toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
        sprintf(fnam, "%s%4d_%03d_%02d_%02d.ps", d1.datadir, yr, da, hr, mn);
        if ((file1 = fopen(fnam, "w")) == NULL) {
            printf("cannot write %s\n", txt);
            return;
        }
        fprintf(file1, "%%!PS-Adobe-\n%c%cBoundingBox:  0 0 612 792\n%c%cEndProlog\n", '%', '%', '%', '%');
        fprintf(file1, "1 setlinewidth\n");
        fprintf(file1, "/Times-Roman findfont\n 10 scalefont\n setfont\n");
        fprintf(file1, "0 0 0 sethsbcolor\n");
    }
    wid = 500.0;
    sx = d1.vwid / wid;
    sy = d1.vwhgt / wid;
    if (d1.debug)
        printf("in vplot\n");
    freqsep = d1.bw / d1.nfreq;
    dmax = ddmax = -1.0e99;
    dmin = 1.0e99;
    dd = sigma = 0.0;
    np = d1.nfreq * d1.fbw / d1.bw + 0.5;
// printf("np %d %d\n",np,(int) (d1.f1 * d1.nfreq));
    j1 = np * 0.0;
    j2 = np * 1.0;
    jmax = 0;
    npoint = 2048;
    av = avx = avy = avxx = avxy = 0.0;
    for (j = 0; j < np; j++) {
        pp[j] = aavspec[j + (int) (d1.f1 * d1.nfreq)];
        if (j < j1 || j > j2 - 1)
            pp[j] = 0.0;
        dd = pp[j];
        if (j >= j1 && j < j2) {
            avx += j;
            avy += dd;
            avxx += j * j;
            avxy += j * dd;
            av++;
            if (dd > ddmax)
                ddmax = dd;
        }
    }
    slope = (-avx * avy + av * avxy) / (av * avxx - avx * avx);
    for (j = j1; j < j2; j++) {
        if (np > 1)
            pp[j] -= slope * (double) (j - j1) / ((double) (j2 - j1) - 1.0);
        dd = pp[j];
        if (dd > dmax) {
            dmax = dd;
            jmax = j;
        }
        if (dd < dmin)
            dmin = dd;
    }
    xoffset = 80.0;
    yoffset = 50.0;
    yps = 450.0;                // was 420
    if (dmax > dmin)
        scale = 1.2 * (dmax - dmin);
    else
        scale = 1.0;
    if (freqsep > 0.0)
        xx = floor(d1.freq / freqsep) * freqsep;
    else
        xx = d1.freq;

    if (np > 1) {
        sprintf(txt, "integ. %5.1f m", (d1.integ * d1.nsam) / (2.0e6 * d1.bw * 60.0));
        sigma = 3.0 * d1.tsys / sqrt((d1.nsam * d1.integ / (2.0e6 * d1.bw)) * freqsep * 1e6);
        if (d1.bsw != 0)
            sigma *= 2.0;
        x1 = (xoffset + 200.0) * sx;
        y1 = (yoffset + 15.0) * sy;
        gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
        psx1 = x1 / sx;
        psy1 = yps - y1 / sy;
        if (d1.psw)
            fprintf(file1, "%f %f moveto\n (%s) show\n", psx1, psy1, txt);
        if (soutrack[0] > 0) {
            sprintf(txt, "%s", soutrack);
            x1 = (xoffset + 20.0) * sx;
            y1 = (yoffset + 15.0) * sy;
            gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            if (d1.psw)
                fprintf(file1, "%f %f moveto\n (%s) show\n", psx1, psy1, txt);
            if (!strstr(soutrack, "Sun") && !strstr(soutrack, "Moon")) {
                sprintf(txt, "Galactic l = %3.0f b = %3.0f", d1.glon, d1.glat);
                x1 = (xoffset + 60.0) * sx;
                y1 = (yoffset + 15.0) * sy;
                gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
                psx1 = x1 / sx;
                psy1 = yps - y1 / sy;
                if (d1.psw)
                    fprintf(file1, "%f %f moveto\n (%s) show\n", psx1, psy1, txt);
            }
        }
        for (y = 0; y < 2; y++) {
            x1 = xoffset * sx;
            y1 = (yoffset + y * 319) * sy;
            x2 = (xoffset + 320) * sx;
            y2 = (yoffset + y * 319) * sy;
            gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y2);
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            psx2 = x2 / sx;
            psy2 = yps - y2 / sy;
            if (d1.psw)
                fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1, psy1,
                        psx2, psy2);
            x1 = (xoffset + y * 320) * sx;
            y1 = (yoffset) * sy;
            x2 = (xoffset + y * 320) * sx;
            y2 = (yoffset + 319) * sy;
            gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y2);
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            psx2 = x2 / sx;
            psy2 = yps - y2 / sy;
            if (d1.psw)
                fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1, psy1,
                        psx2, psy2);
        }

        yp = 0;
        xp = (npoint - 1) * 320.0 / (double) npoint;
// spec plot start
        for (j = 1; j < npoint; j++) {
            x = (npoint - j) * 320.0 / (double) npoint;
            xx = j / (double) npoint;
            k = (int) (xx * (double) np + 0.5);
            if (k >= np)
                k = np - 1;
            if (scale > 0.0)
                totpp = (pp[k] - dmin) / scale;
            else
                totpp = 0;
            y = (260.0 - totpp * 260.0);
            if (y < 0)
                y = 0;
            if (y > 260)
                y = 260;
            if (j == 1)
                yp = y;
            if (y != yp) {
                if (k >= j1 + 1 && k <= j2 - 1)
                    i = 1;
                else
                    i = 0;
                if (i) {
                    x1 = (x + xoffset) * sx;
                    y1 = (yoffset + yp) * sy;
                    x2 = (xp + xoffset) * sx;
                    gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y1);
                    psx1 = x1 / sx;
                    psy1 = yps - y1 / sy;
                    psx2 = x2 / sx;
                    psy2 = yps - y2 / sy;
                    if (d1.psw)
                        fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1,
                                psy1, psx2, psy1);
                }
                xp = x;
                if (y > yp && i) {
                    x1 = (x + xoffset) * sx;
                    y1 = (yoffset + yp) * sy;
                    y2 = (yoffset + y) * sy;
                    gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x1, y2);
                    psx1 = x1 / sx;
                    psy1 = yps - y1 / sy;
                    psx2 = x2 / sx;
                    psy2 = yps - y2 / sy;
                    if (d1.psw)
                        fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1,
                                psy1, psx1, psy2);
                }
                if (yp > y && i) {
                    x1 = (x + xoffset) * sx;
                    y1 = (yoffset + y) * sy;
                    y2 = (yoffset + yp) * sy;
                    gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x1, y2);
                    psx1 = x1 / sx;
                    psy1 = yps - y1 / sy;
                    psx2 = x2 / sx;
                    psy2 = yps - y2 / sy;
                    if (d1.psw)
                        fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1,
                                psy1, psx1, psy2);
                }
            }
            yp = y;
        }
//  plot end
        fstart = d1.freq + (double) (1 - np / 2) * freqsep;
        fstop = d1.freq + (double) (np - 1 - np / 2) * freqsep;
        vstart = -d1.vlsr - (fstop - d1.restfreq) * 299790.0 / d1.restfreq;
        vstop = -d1.vlsr - (fstart - d1.restfreq) * 299790.0 / d1.restfreq;
        ddd = fstop - fstart;
        n3 = (int) (ddd) + 1;
        ddd = 10.0 / n3;
        j1 = (int) (fstart * ddd);
        j2 = (int) (fstop * ddd);
        if (d1.psw)
            fprintf(file1, "/Times-Roman findfont\n 10 scalefont\n setfont\n");
        for (j = j1 + 1; j <= j2; j++) {
            dd = ((double) (j) / ddd - d1.freq + (double) (np / 2) * freqsep)
                * 320.0 / ((double) (np) * freqsep);
            x1 = (320 - dd + xoffset) * sx;
            y1 = (yoffset + 310.0) * sy;
            x2 = (320.0 - dd + xoffset) * sx;
            y2 = (yoffset + 319.0) * sy;
            gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y2);
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            psx2 = x2 / sx;
            psy2 = yps - y2 / sy;
            if (d1.psw)
                fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1, psy1,
                        psx2, psy2);
            sprintf(txt, "%6.1f", j / ddd);
            x1 = (310 - dd + xoffset) * sx;
            y1 = (yoffset + 335.0) * sy;
            gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            if (d1.psw)
                fprintf(file1, "%f %f moveto\n (%s) show\n", psx1, psy1, txt);
            sprintf(txt, "Frequency (MHz)");
            x1 = (125.0 + xoffset) * sx;
            y1 = (yoffset + 350.0) * sy;
            gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            if (d1.psw)
                fprintf(file1, "%f %f moveto\n (%s) show\n", psx1, psy1, txt);
        }
        ddd = 10.0 * n3;
        j1 = (int) (vstart / ddd);
        j2 = (int) (vstop / ddd);
        if (d1.psw)
            fprintf(file1, "/Times-Roman findfont\n 8 scalefont\n setfont\n");
        for (j = j1 + 1; j < j2; j++) {
            freq = d1.restfreq - ((double) (j) * ddd + d1.vlsr) * d1.restfreq / 299790.0;
            dd = (freq - d1.freq + (double) (np / 2) * freqsep)
                * 320.0 / ((double) (np) * freqsep);
            x1 = (320 - dd + xoffset) * sx;
            y1 = (yoffset + 265.0) * sy;
            x2 = (320 - dd + xoffset) * sx;
            y2 = (yoffset + 275.0) * sy;
            gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y2);
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            psx2 = x2 / sx;
            psy2 = yps - y2 / sy;
            if (d1.psw)
                fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1, psy1,
                        psx2, psy2);
            sprintf(txt, "%5.0f", j * ddd);
            x1 = (305 - dd + xoffset) * sx;
            y1 = (yoffset + 290.0) * sy;
            gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            if (d1.psw)
                fprintf(file1, "%f %f moveto\n (%s) show\n", psx1 + 5.0, psy1, txt);
            sprintf(txt, "VLSR (km/s)");
            x1 = (135 + xoffset) * sx;
            y1 = (yoffset + 305.0) * sy;
            gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            if (d1.psw)
                fprintf(file1, "%f %f moveto\n (%s) show\n", psx1, psy1, txt);
        }
        if (d1.psw)
            fprintf(file1, "/Times-Roman findfont\n 10 scalefont\n setfont\n");
        dd = log(scale) / log(2.0) - 0.6;
        if (dd < 0.0)
            j = (int) (dd - 0.5);
        else
            j = (int) (dd + 0.5);
        dd = 0.5 * pow(2.0, (double) j);
        j1 = 0;
        j2 = (int) (scale / dd);
        for (j = 0; j <= j2; j++) {
            y = (int) (260.0 - ((double) j * dd / scale) * 260.0);
            if (y > 0) {
                x1 = xoffset * sx;
                y1 = (yoffset + y) * sy;
                x2 = (10 + xoffset) * sx;
                y2 = (yoffset + y) * sy;
                gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y2);
                psx1 = x1 / sx;
                psy1 = yps - y1 / sy;
                psx2 = x2 / sx;
                psy2 = yps - y2 / sy;
                if (d1.psw)
                    fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1, psy1,
                            psx2, psy2);
                sprintf(txt, "%5.1fK", j * dd);
                x1 = (xoffset - 30) * sx;
                y1 = (yoffset + y + 2.0) * sy;
                gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
                psx1 = x1 / sx;
                psy1 = yps - y1 / sy;
                if (d1.psw)
                    fprintf(file1, "%f %f moveto\n (%s) show\n", psx1, psy1, txt);
            }
        }
        yy = (sigma / scale) * 260.0;
        if (yy > 0.0) {
            x1 = (xoffset + 310) * sx;
            y1 = (yoffset + 10) * sy;
            x2 = (xoffset + 310) * sx;
            y2 = (yoffset + yy + 10) * sy;
            gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y2);
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            psx2 = x2 / sx;
            psy2 = yps - y2 / sy;
            if (d1.psw)
                fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1, psy1,
                        psx2, psy2);
            x1 = (xoffset + 305) * sx;
            y1 = (yoffset + 10) * sy;
            x2 = (xoffset + 315) * sx;
            y2 = (yoffset + 10) * sy;
            gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y2);
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            psx2 = x2 / sx;
            psy2 = yps - y2 / sy;
            if (d1.psw)
                fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1, psy1,
                        psx2, psy2);
            x1 = (xoffset + 305) * sx;
            y1 = (yoffset + yy + 10) * sy;
            x2 = (xoffset + 315) * sx;
            y2 = (yoffset + yy + 10) * sy;
            gdk_draw_line(vpixmap, vdrawing_area->style->black_gc, x1, y1, x2, y2);
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            psx2 = x2 / sx;
            psy2 = yps - y2 / sy;
            if (d1.psw)
                fprintf(file1, "newpath\n %5.1f %5.1f moveto \n %5.1f %5.1f lineto\nstroke\n", psx1, psy1,
                        psx2, psy2);
            sprintf(txt, "3-sigma");
            x1 = (xoffset + 270) * sx;
            y1 = (yoffset + yy * 0.5 + 14) * sy;
            gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
            psx1 = x1 / sx;
            psy1 = yps - y1 / sy;
            if (d1.psw)
                fprintf(file1, "%f %f moveto\n (%s) show\n", psx1, psy1, txt);
        }
    }
    if (d1.psw) {
        fprintf(file1, "showpage\n%c%cTrailer\n", '%', '%');
        fclose(file1);
        sprintf(txt, "file: %s written", fnam);
        x1 = (xoffset + 5) * sx;
        y1 = (yoffset + 440) * sy;
        gdk_draw_text(vpixmap, vfixed_font, vdrawing_area->style->black_gc, x1, y1, txt, strlen(txt));
    }
}
