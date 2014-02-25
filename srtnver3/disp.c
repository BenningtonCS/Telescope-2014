#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"
#include "hproto.h"

void button_clear_clicked(void)
{
    d1.ppos = 0;
    d1.clearint = 1;
}

void button_stow_clicked(void)
{
    d1.azcmd = d1.azlim1;
    d1.elcmd = d1.ellim1;
    soutrack[0] = 0;
    d1.bsw = 0;
    d1.scan = 0;
    d1.stow = 1;
}

void button_azel_clicked(void)
{
    if (d1.entry1)
        return;
    entry1 = gtk_entry_new();
    d1.entry1 = 1;
    d1.stopproc = 1;
    gtk_table_attach(GTK_TABLE(table), entry1, 0, NUMBUTTONS, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(entry1);
    g_signal_connect(G_OBJECT(entry1), "activate", G_CALLBACK(hit_enter_azel), NULL);
    gtk_entry_set_text(GTK_ENTRY(entry1), "enter az el: ");
    gtk_entry_set_position(GTK_ENTRY(entry1), 12);
    gtk_entry_set_visibility(GTK_ENTRY(entry1), TRUE);
}

void hit_enter_azel(void)
{
    const gchar *str;
    if (!d1.entry1)
        return;
    str = gtk_entry_get_text(GTK_ENTRY(entry1));
    if (d1.debug)
        printf("str=%s\n", str);
    if (str != NULL)
        sscanf(str, "%*s %*s %*s %lf %lf", &d1.azcmd, &d1.elcmd);
    if (d1.debug)
        printf("cmd %f %f\n", d1.azcmd, d1.elcmd);
    d1.track = 0;
    soutrack[0] = 0;
    gtk_widget_destroy(entry1);
    d1.entry1 = 0;
    d1.stopproc = 0;
}

void button_freq_clicked(void)
{
    if (d1.entry3)
        return;
    entry3 = gtk_entry_new();
    d1.entry3 = 1;
    gtk_table_attach(GTK_TABLE(table), entry3, 0, NUMBUTTONS, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(entry3);

    g_signal_connect(G_OBJECT(entry3), "activate", G_CALLBACK(hit_enter_freq), NULL);
    gtk_entry_set_text(GTK_ENTRY(entry3), "enter frequency: ");
    d1.stopproc = 1;
    gtk_entry_set_position(GTK_ENTRY(entry3), 17);
    gtk_entry_set_visibility(GTK_ENTRY(entry3), TRUE);
}

void hit_enter_freq(void)
{
    const gchar *str;
    double freq, bw;
    int n;
    freq = 1420;
    bw = d1.bw;
    n = d1.nfreq;
    if (!d1.entry3)
        return;
    str = gtk_entry_get_text(GTK_ENTRY(entry3));
    if (str != NULL)
        sscanf(str, "%*s %*s %lf %lf %d", &freq, &bw, &n);
    if (bw > 0 && bw <= 10.0)
        if (freq > 1200.0 && freq < 1800.0) {
            d1.freq = freq;
            d1.nfreq = n;
            if (d1.nfreq > NSPEC || d1.nfreq < 1)
                d1.nfreq = NSPEC;
            d1.f1 = 0.5 - d1.bw * 0.5;
            d1.f2 = 0.5 + d1.bw * 0.5;
            d1.fc = (d1.f1 + d1.f2) * 0.5;
            if (d1.debug)
                printf("new freq %f %d\n", d1.freq, d1.nfreq);
            d1.freqchng = 1;
            d1.calpwr = 0;
        }
    d1.stopproc = 0;
    gtk_entry_set_text(GTK_ENTRY(entry3), "");
    gtk_widget_destroy(entry3);
    d1.entry3 = 0;
}

void button_offset_clicked(void)
{
    if (d1.entry2)
        return;
    entry2 = gtk_entry_new();
    d1.entry2 = 1;
    gtk_table_attach(GTK_TABLE(table), entry2, 0, NUMBUTTONS, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(entry2);

    g_signal_connect(G_OBJECT(entry2), "activate", G_CALLBACK(hit_enter_offset), NULL);
    gtk_entry_set_text(GTK_ENTRY(entry2), "enter offsets: ");
    d1.stopproc = 1;
    gtk_entry_set_position(GTK_ENTRY(entry2), 14);
    gtk_entry_set_visibility(GTK_ENTRY(entry2), TRUE);
}

void hit_enter_offset(void)
{
    const gchar *str;
    if (!d1.entry2)
        return;
    str = gtk_entry_get_text(GTK_ENTRY(entry2));
    if (str != NULL)
        sscanf(str, "%*s %*s %lf %lf", &d1.azoff, &d1.eloff);
    d1.stopproc = 0;
    gtk_entry_set_text(GTK_ENTRY(entry2), "");
    gtk_widget_destroy(entry2);
    d1.entry2 = 0;
}

void button_npoint_clicked(void)
{
    if (d1.scan == 0 && !d1.stow) {
        d1.scan = 1;
        gtk_tooltips_set_tip(tooltips, button_npoint, "npoint running", NULL);
    } else {
        d1.scan = 0;
        d1.eloff = 0;
        gtk_tooltips_set_tip(tooltips, button_npoint, "click to start npoint scan", NULL);
    }
    d1.bsw = 0;
    d1.azoff = 0;
}

void button_bsw_clicked(void)
{
    if (d1.bsw == 0 && !d1.stow) {
        d1.bsw = 1;
        gtk_tooltips_set_tip(tooltips, button_bsw, "beam switch running - click again to stop", NULL);
    } else {
        d1.bsw = 0;
        gtk_tooltips_set_tip(tooltips, button_bsw, "click to start beam switch", NULL);
        d1.azoff = 0;
    }
}

void button_record_clicked(void)
{
    char txt[80];
    GdkColor color;
    if (d1.record) {
        d1.record = 0;
        d1.foutstatus = 0;
        color.green = 0xffff;
        color.red = 0xffff;
        color.blue = 0xffff;
        gtk_widget_modify_bg(button_record, GTK_STATE_NORMAL, &color);
    } else {
        d1.record = 1;

        if (d1.entry6)
            return;
        entry6 = gtk_entry_new();
        d1.entry6 = 1;
        gtk_table_attach(GTK_TABLE(table), entry6, 0, NUMBUTTONS, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
        gtk_widget_show(entry6);

        g_signal_connect(G_OBJECT(entry6), "activate", G_CALLBACK(hit_enter_record), NULL);
        sprintf(txt, "enter record file name (enter blank to accept assigned name): ");
        gtk_entry_set_text(GTK_ENTRY(entry6), txt);
        d1.stopproc = 1;
        gtk_entry_set_position(GTK_ENTRY(entry6), 61); // has to match above
        gtk_entry_set_visibility(GTK_ENTRY(entry6), TRUE);

    }
    record_tooltip();
}

void hit_enter_record(void)
{
    const gchar *str;
    char txt[80];
    if (!d1.entry6)
        return;
    str = gtk_entry_get_text(GTK_ENTRY(entry6));
    txt[0] = d1.filname[0] = 0;
    if (str != NULL)
        sscanf(str, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %24s", txt);
    if (txt[0]) {
        sscanf(txt, "%24s", d1.filname);
    }
    d1.stopproc = 0;
    gtk_entry_set_text(GTK_ENTRY(entry6), "");
    gtk_widget_destroy(entry6);
    d1.entry6 = 0;
}

void record_tooltip(void)
{

    if (d1.record)
        gtk_tooltips_set_tip(tooltips, button_record, "click to stop recording", NULL);
    else
        gtk_tooltips_set_tip(tooltips, button_record, "click to start recording", NULL);
}

void button_cmdfl_clicked(void)
{
    char txt[80];
    FILE *file1;
    if (d1.debug)
        printf("cmdfl\n");
    if (d1.cmdfl) {
        sprintf(txt, "cmdfile: %s stopped", d1.cmdfnam);
        gtk_tooltips_set_tip(tooltips, button_cmdfl, txt, NULL);
        d1.cmdfl = 0;
        return;
    }
    d1.cmdfl = 1;
    d1.cmdfline = 0;
    d1.secstop = 0;
    if ((file1 = fopen(d1.cmdfnam, "r")) == NULL) {
        if (d1.debug)
            printf(" Unable to open %s\n", d1.cmdfnam);
        if (d1.entry5)
            return;
        entry5 = gtk_entry_new();
        d1.entry5 = 1;
        gtk_table_attach(GTK_TABLE(table), entry5, 0, NUMBUTTONS, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
        gtk_widget_show(entry5);

        g_signal_connect(G_OBJECT(entry5), "activate", G_CALLBACK(hit_enter_cmdfl), NULL);
        sprintf(txt, "unable to open %s enter cmd file name: ", d1.cmdfnam);
        gtk_entry_set_text(GTK_ENTRY(entry5), txt);
        d1.stopproc = 1;
        gtk_entry_set_position(GTK_ENTRY(entry5), 14);
        gtk_entry_set_visibility(GTK_ENTRY(entry5), TRUE);
    } else {
        fclose(file1);
        sprintf(txt, "cmdfile: %s activated", d1.cmdfnam);
        gtk_tooltips_set_tip(tooltips, button_cmdfl, txt, NULL);
    }
}

void hit_enter_cmdfl(void)
{
    FILE *file1;
    const gchar *str;
    char txt[80];
    if (!d1.entry5)
        return;
    str = gtk_entry_get_text(GTK_ENTRY(entry5));
    if (str != NULL)
        sscanf(str, "%*s %*s %*s %*s %*s %*s %*s %*s %24s", d1.cmdfnam);
    d1.stopproc = 0;
    if ((file1 = fopen(d1.cmdfnam, "r")) == NULL) {
        d1.cmdfl = 0;
    } else {
        fclose(file1);
        sprintf(txt, "cmdfile: %s activated", d1.cmdfnam);
        gtk_tooltips_set_tip(tooltips, button_cmdfl, txt, NULL);
    }
    gtk_entry_set_text(GTK_ENTRY(entry5), "");
    gtk_widget_destroy(entry5);
    d1.entry5 = 0;
}


/*
void button_cal_clicked(void)
{
    d1.docal = 1;
}
*/

void button_cal_clicked(void)
{
    GdkColor color;
    if (d1.caldone) {
        d1.caldone = 0;
        color.green = 0xffff;
        color.red = 0xffff;
        color.blue = 0xffff;
        gtk_widget_modify_bg(button_cal, GTK_STATE_NORMAL, &color);
    }
    if (d1.entry8)
        return;
    if (d1.calmode == 1) {
        d1.calpwr = 0;
        return;
    }
    if (d1.calmode == 3) {
        d1.docal = 1;
        return;
    }
    entry8 = gtk_entry_new();
    d1.entry8 = 1;
    d1.stopproc = 1;
    gtk_table_attach(GTK_TABLE(table), entry8, 0, NUMBUTTONS, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(entry8);
    g_signal_connect(G_OBJECT(entry8), "activate", G_CALLBACK(hit_enter_cal), NULL);
    if (d1.calmode == 0 || d1.calmode == 20)
        gtk_entry_set_text(GTK_ENTRY(entry8), "place absorber enter: ");
    if (d1.calmode == 2)
        gtk_entry_set_text(GTK_ENTRY(entry8), "enter to start bandpass cal: ");
    gtk_entry_set_position(GTK_ENTRY(entry8), 22);
    gtk_entry_set_visibility(GTK_ENTRY(entry8), TRUE);
}

void hit_enter_cal(void)
{
    const gchar *str;
    if (!d1.entry8)
        return;
    str = gtk_entry_get_text(GTK_ENTRY(entry8));
    if (d1.debug)
        printf("str=%s\n", str);
    d1.docal = 1;               // default
    if (str != NULL)
        sscanf(str, "%*s %*s %*s %d", &d1.docal);
//    d1.track = 0;   don't need to stop track
//    soutrack[0] = 0;
    d1.stopproc = 0;
    gtk_entry_set_text(GTK_ENTRY(entry8), "");
    gtk_widget_destroy(entry8);
    d1.entry8 = 0;

}

void button_help_clicked(void)
{
    display_help();
}

void button_exit_clicked(void)
{
    if (d1.stow == -1)
        d1.run = 0;
    else {
        if (d1.slew) {
            d1.track = -1;
            soutrack[0] = 0;
            gtk_tooltips_set_tip(tooltips, button_exit, "antenna stopped - go to stow", NULL);
        } else
            gtk_tooltips_set_tip(tooltips, button_exit, "first go to stow", NULL);
    }
}

gint button_press_event(GtkWidget * widget, GdkEventButton * event)
{
//  GdkModifierType state;
    int x, y, i, imin;
    double secs, az, el, ra, dec, min, az2, el2, daz, del, r;
    if (event->button == 1 && pixmap != NULL) {
//    gdk_window_get_pointer (event->window, &x, &y, &state)
        x = event->x;
        y = event->y;
    } else
        return TRUE;
    if (d1.debug)
        printf("x= %d y= %d midx %d midy %d width %d\n", x, y, midx, midy, widget->allocation.width);
    secs = d1.secs;
    min = 1e99;
    imin = -1;
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
        } else
            radec_azel(gst(secs) - ras[i] - d1.lon, decs[i], d1.lat, &az, &el);
        az = az * 180.0 / PI;
        el = el * 180.0 / PI;
        if (d1.south)
            az2 = 180.0 * x / ((double) midx);
        else {
            az2 = (x - midx) * 180.0 / ((double) midx);
        }
        el2 = (midy * 2 - y) * 180.0 / (((double) midy) * 2.0);
        daz = az2 - az;
        del = el2 - el;
        if (daz > 180.0)
            daz -= 360.0;
        if (daz < -180.0)
            daz += 360.0;
        r = daz * daz * cos(el * PI / 180.0) * cos(el * PI / 180.0) + del * del;
        if (r < min && el >= 0) // select only source above horizon
        {
            min = r;
            imin = i;
        }
    }
    if (imin >= 0 && min < 40.0 * 40.0 && y >= midy) {
        strncpy(soutrack, sounam[imin], sizeof(soutrack) - 1);
        if (d1.debug)
            printf("track %s i=%d\n", soutrack, imin);
        d1.track = 1;
        if (d1.scan) {
            d1.scan = 0;
            d1.azoff = d1.eloff = 0.0;
        }
        if (d1.bsw) {
            d1.bsw = 0;
            d1.azoff = 0.0;
        }
    } else if (y <= midy && x >= midx)
        velspec();

    return TRUE;
}


void clearpaint(void)
{
//  GtkWidget *drawing_area = (GtkWidget *) data;
    GdkRectangle update_rect;
    gdk_draw_rectangle(pixmap,
                       drawing_area->style->white_gc, TRUE, 0, 0,
                       drawing_area->allocation.width, drawing_area->allocation.height);
    update_rect.x = 0;
    update_rect.y = 0;
    update_rect.width = drawing_area->allocation.width;
    update_rect.height = drawing_area->allocation.height;
    midx = drawing_area->allocation.width / 2;
    midy = drawing_area->allocation.height / 2;
//    gtk_widget_draw(drawing_area, &update_rect);
//  gtk_widget_queue_draw_area(drawing_area,update_rect.x,update_rect.y,update_rect.width,update_rect.height);
}

void cleararea(void)
{
    gdk_draw_rectangle(pixmap,
                       drawing_area->style->white_gc, TRUE, 0, midy, drawing_area->allocation.width, midy);
    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, midx, 0, midx, midy * 0.95);
    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, midx * 1.5, midy * 0.95,
                       midx * 0.5, midy * 0.05);
}

gint configure_event(GtkWidget * widget)
{
    int newp = 0;
    int sz = 140;
    char txt[80];
    GtkSettings *settings;
    if (!pixmap || d1.wid != widget->allocation.width || d1.whgt != widget->allocation.height)
        newp = 1;
// if (pixmap && newp)  printf("new pixmap\n");   // only occurs on resize
    if (pixmap && newp)
        gdk_pixmap_unref(pixmap);
    if (newp) {

        sz = (widget->allocation.width / 72) * 10; // font = 110 with wid = 800  - works best over range of size
//  printf("sz %d wid %d %d %d\n",sz,widget->allocation.width,midx,drawing_area->allocation.width);
        if (sz > 140)
            sz = 140;
        sprintf(txt, "-misc-fixed-medium-r-*-*-*-%d-*-*-*-*-iso8859-*", sz); // specified iso to avoid 16-bit encode
// look into difference between  gdk_font_load and  gdk_fontset_load
//  printf("%s\n",txt);
//  sprintf(txt,"-adobe-helvetica-bold-r-normal--12-120-75-75-p-70-iso8859-1");
//  sprintf(txt,"-adobe-helvetica-medium-r-normal--*-%d-*-*-*-*-iso8859-1",sz);
        fixed_font = gdk_font_load(txt);
        settings = gtk_settings_get_default();
        sz = sz / 11;
        if (sz < 1)
            sz = 1;
        sprintf(txt, "Sans %d", sz);
        g_object_set(G_OBJECT(settings), "gtk-font-name", txt, NULL);
//  gtk_table_resize(GTK_TABLE(table), 10, 12);
//  gtk_table_set_row_spacing(GTK_TABLE(table),0,20);

        pixmap = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
        gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, 0, 0,
                           widget->allocation.width, widget->allocation.height);
        clearpaint();
        d1.wid = widget->allocation.width;
        d1.whgt = widget->allocation.height;
        if (d1.debug)
            printf("wid %d whgt %d sz %d\n", d1.wid, d1.whgt, sz);
    }
    return TRUE;
}

gint expose_event(GtkWidget * widget, GdkEventExpose * event)
{
    gdk_draw_pixmap(widget->window,
                    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                    pixmap,
                    event->area.x, event->area.y,
                    event->area.x, event->area.y, event->area.width, event->area.height);
//  printf("draw pixmap\n");   // the above actually draws to screen
    return FALSE;
}

void quit()
{
    gtk_exit(0);
}
