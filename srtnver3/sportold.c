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

// com1 0x3f8 com2 0x2f8
#define KKTIMEOUT 360

void azel(double az, double el)
  // command antenna movement
{
    int kk, n, mm, ax, axis, count, ccount, rcount, flip;
    int ix, iy, midxr, ixe;
    int yr, da, hr, mn, sc;
    int region1, region2, region3;
    unsigned int base;
    double azscale, elscale, azz, ell, ra, dec, x, y;
    double elnow, lenzero, elcount;
    GdkColor color;
    int azatstow = 0;
    int elatstow = 0;
    char m[80], recv[80], str[80], str2[80], txt[80];

    mm = count = 0;
    d1.slew = 0;
    lenzero = 0;

    ix = midx * 1.55;
    ixe = midx * 0.25;
    midxr = midx * 2 - ix;
    if (d1.lat >= 0.0)
        sprintf(txt, "%s %4.1fN %5.1fW", d1.statnam, d1.lat * 180.0 / PI, d1.lon * 180.0 / PI);
    else
        sprintf(txt, "%s %4.1fS %5.1fW", d1.statnam, -d1.lat * 180.0 / PI, d1.lon * 180.0 / PI);
    iy = midy * 0.05;
    if (d1.displ) {
        gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                           iy - midy * 0.04, midxr, midy * 0.05);
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    }
    sprintf(txt, "cmd  %5.1f %4.1f deg", d1.azcmd, d1.elcmd);
    iy = midy * 0.15;
    if (d1.displ)
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    else if (d1.debug) {
        toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
        printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
        printf("%s\n", txt);
    }
    sprintf(txt, "offsets %5.1f %4.1f deg", d1.azoff, d1.eloff);
    iy = midy * 0.25;
    if (d1.displ)
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));

    base = d1.azelport;

    if (d1.azelsim == 0) {
        outb(128 + 3, base + 3);
        outb(0x30, base);
        outb(0, base + 1);      // 2400 8 data 1 stop noparity
        outb(3, base + 3);
        outb(7, base + 1);
        outb(1, base + 2);
        outb(3, base + 4);
        sprintf(str, "antenna drive status:");
        if (d1.comerr > 0) {
            sprintf(txt, " comerr= %d", d1.comerr);
            iy = midy * 0.1;
            if (d1.displ) {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                   iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
            }
        }
    } else {
        sprintf(txt, "antenna simulated");
        iy = midy * 0.1;
        if (d1.displ) {
            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                               iy - midy * 0.04, midxr, midy * 0.05);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        }
    }
    if (az > 360.0)
        az -= 360.0;            /* Fold into reasonable range */
    if (d1.south == 0) {
        az = az + 360.0;        /* put az in range 180 to 540 */
        if (az > 540.0)
            az -= 360.0;
        if (az < 180.0)
            az += 360.0;
    }
    region1 = region2 = region3 = 0;
    if (az >= d1.azlim1 && az < d1.azlim2 && el >= d1.ellim1 && el <= d1.ellim2)
        region1 = 1;
    if (az > d1.azlim1 + 180.0 && el > (180.0 - d1.ellim2))
        region2 = 1;
    if (az < d1.azlim2 - 180.0 && el > (180.0 - d1.ellim2))
        region3 = 1;
    if (!region1 && !region2 && !region3) {
        sprintf(txt, "cmd out of limits");
        iy = midy * 0.10;
        if (d1.displ) {
            color.red = 0xffff;
            color.green = color.blue = 0;
            gdk_color_parse("red", &color);
            gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);

            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                               iy - midy * 0.04, midxr, midy * 0.05);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->fg_gc[GTK_STATE_NORMAL], ix,
                          iy, txt, strlen(txt));
        }
        d1.track = 0;
        if (d1.printout) {
            toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
            printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
            printf("cmd out of limits az %f el %f\n", az, el);
        }
        if (d1.stow != -1) {
            d1.stow = 1;
            d1.elcmd = d1.ellim1;
            d1.azcmd = d1.azlim1;
        }
        return;
    }
    flip = 0;
    if (az > d1.azlim2) {
        az -= 180.0;
        el = 180.0 - el;
        flip = 1;
    }
    if (az < d1.azlim1 && flip == 0) {
        az += 180.0;
        el = 180.0 - el;
        flip = 1;
    }
    azz = az - d1.azlim1;
    ell = el - d1.ellim1;
//    scale = 52.0 * 27.0 / 120.0;
    azscale = d1.azcounts_per_deg;
    elscale = d1.elcounts_per_deg;
    if (!d1.rod)
        elcount = ell * elscale;
    else {
        lenzero = d1.rod1 * d1.rod1 + d1.rod2 * d1.rod2
            - 2.0 * d1.rod1 * d1.rod2 * cos((d1.rod4 - d1.ellim1) * PI / 180.0) - d1.rod3 * d1.rod3;
        if (lenzero >= 0.0)
            lenzero = sqrt(lenzero);
        else
            lenzero = 0;
        elcount =
            d1.rod1 * d1.rod1 + d1.rod2 * d1.rod2 - 2.0 * d1.rod1 * d1.rod2 * cos((d1.rod4 - el) * PI / 180.0)
            - d1.rod3 * d1.rod3;
        if (elcount >= 0.0)
            elcount = (-sqrt(elcount) + lenzero) * d1.rod5;
        else
            elcount = 0;
// increase in length drives to horizon
    }
/* mm=1=clockwize incr.az mm=0=ccw mm=2= down when pointed south */
    if ((int) elcount > d1.elcount)
        axis = 1;               // move in elevation first

    else
        axis = 0;
    for (ax = 0; ax < 2; ax++) {
        if (axis == 0) {
            if (azz * azscale > d1.azcount - 0.5) {
                mm = 1;
                count = (int) (azz * azscale - d1.azcount + 0.5);
            }
            if (azz * azscale <= d1.azcount + 0.5) {
                mm = 0;
                count = (int) (d1.azcount - azz * azscale + 0.5);
            }
        } else {
            if (elcount > d1.elcount - 0.5) {
                mm = 3;
                count = (int) (elcount - d1.elcount + 0.5);
            }
            if (elcount <= d1.elcount + 0.5) {
                mm = 2;
                count = (int) (d1.elcount - elcount + 0.5);
            }
        }
        ccount = count;
        if (d1.stow == 1 && d1.azcmd == d1.azlim1 && d1.elcmd == d1.ellim1) // drive to stow

        {
            count = 5000;
            if (axis == 0) {
                mm = 0;
                if (azatstow == 1)
                    count = 0;
            }
            if (axis == 1) {
                mm = 2;
// complete azimuth motion to stow before completely drop in elevation
                if (elatstow == 1 || (ccount <= 2.0 * d1.countperstep && azatstow == 0))
                    count = 0;
            }
            flip = 0;
        }
        if (count > d1.countperstep && ccount > d1.countperstep)
            count = d1.countperstep;
        if (count >= d1.ptoler && d1.track != -1) {
            if (count > d1.ptoler && d1.stow != -1) {
                d1.slew = 1;
                sprintf(txt, "ant slewing");
                if (d1.printout) {
                    toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
                    printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
                    if (soutrack[0]) {
                        if (axis)
                            printf("ant slewing in el to %s\n", soutrack);
                        else
                            printf("ant slewing in az to %s\n", soutrack);
                    }
                    if (d1.stow == 1) {
                        if (axis)
                            printf("ant slewing in el to stow\n");
                        else
                            printf("ant slewing in az to stow\n");
                    }
                }
                iy = midy * 0.1;
                if (d1.displ) {
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc,
                                       TRUE, ix, iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_text(pixmap, fixed_font,
                                  drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                }
            }

            if (d1.displ) {
                x = d1.azcmd * midx / 180.0;
                if (d1.south == 0)
                    x -= midx;
                if (x < 0)
                    x += midx * 2;
                if (x > midx * 2)
                    x -= midx * 2;
                y = midy * 2 - d1.elcmd * midy * 2.0 / 180.0;
                color.red = 0;
                color.green = 0;
                color.blue = 0xffff;
                gdk_color_parse("blue", &color);
                gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);
                gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x - 4, y, x + 4, y);
                gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x, y - 4, x, y + 4);
            }
            sprintf(str, " move %d %d \n", mm, count); /* need space at start and end */
            n = 0;
            if (d1.azelsim != 0) {
                if (count < 5000)
                    sprintf(str2, "M %d \n", count);
                else
                    sprintf(str2, "T %d \n", count);
                strcpy(recv, str2);
//          n = str2.length();
            }
//        d.dtext(16.0, 64.0, gg, Color.black,
//                "trans " + str.substring(0, str.length() - 1) + "     ");
            kk = 0;
            {
                if (d1.azelsim == 0) {
                    while ((inb(base + 2) & 1) == 0 && kk < KKTIMEOUT) {
                        inb(base);
                        kk++;
                    }
                    strcpy(m, str);
//  printf("sending\n");
                    for (n = 0; (unsigned) n < strlen(m); n++) {
                        outb(m[n], base);
                        while ((inb(base + 5) & 32) != 32);
                    }
                    if (d1.debug)
                        printf("sent %s", m);
                    for (n = 0; n < 80; n++)
                        recv[n] = 0;
                    kk = 0;
                    n = 0;
                    rcount = 0;
                }
                while (kk < KKTIMEOUT && kk >= 0) {
                    if (d1.azelsim == 0) {
                        while ((inb(base + 5) & 1) != 0) {
                            recv[n] = inb(base);
//       printf("n=%d recv=%1c val=%d\n",n,recv[n],recv[n]);
                            if (d1.displ == 0)
                                printf("waiting %d\n", kk);
                            if (recv[n] == 13) {
                                kk = -99;
                                break;
                            }
//        if (d1.displ == 0)
//   printf ("n=%d ch=%1c\n", n, recv[n]);
                            n++;
                        }
                    } else if (kk > 1)
                        kk = -99;
                    if (kk > 0) {
                        if (axis == 0)
                            sprintf(txt, "waiting on azimuth  %d  ", kk);
                        else
                            sprintf(txt, "waiting on elevation %d ", kk);
                        iy = midy * 0.1;
                        d1.slew = 1;
                        if (d1.displ) {
                            ix = midx * 1.55;
                            midxr = midx * 2 - ix;
//                            cleararea();
                            x = d1.azcmd * midx / 180.0;
                            if (d1.south == 0)
                                x -= midx;
                            if (x < 0)
                                x += midx * 2;
                            if (x > midx * 2)
                                x -= midx * 2;
                            y = midy * 2 - d1.elcmd * midy * 2.0 / 180.0;
                            color.red = 0;
                            color.green = 0;
                            color.blue = 0xffff;
                            gdk_color_parse("blue", &color);

                            gtk_widget_modify_fg(drawing_area, GTK_STATE_NORMAL, &color);
                            gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x - 4,
                                          y, x + 4, y);
                            gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x,
                                          y - 4, x, y + 4);


                            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                                               iy - midy * 0.04, midxr, midy * 0.05);
                            gdk_draw_text(pixmap, fixed_font,
                                          drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                            sprintf(txt, "cmd  %5.1f %4.1f deg", d1.azcmd, d1.elcmd);
                            iy = midy * 0.15;
                            gdk_draw_text(pixmap, fixed_font,
                                          drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                            sprintf(txt, "azel %5.1f %4.1f deg", d1.aznow, d1.elnow);
                            iy = midy * 0.20;
                            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy,
                                          txt, strlen(txt));
                            sprintf(txt, "offsets %5.1f %4.1f deg", d1.azoff, d1.eloff);
                            iy = midy * 0.25;
                            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy,
                                          txt, strlen(txt));
                            azel_to_radec(d1.secs, d1.aznow, d1.elnow, &ra, &dec);
                            sprintf(txt, "ra %5.1f hr %4.1f deg", ra * 12.0 / PI, dec * 180.0 / PI);
                            iy = midy * 0.30;
                            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy,
                                          txt, strlen(txt));
                            color.green = color.red = color.blue = 0xffff;
                            gtk_widget_modify_bg(button_stow, GTK_STATE_NORMAL, &color);
                            if (!d1.plot) {
                                Repaint();
                            }
//                            cleararea();
                            while (gtk_events_pending() || d1.stopproc) {
                                gtk_main_iteration();
//                                 d1.plot = 0;
                            }

                        } else if (d1.debug)
                            printf("%s\n", txt);
                        sleep(1);
                    }
                    kk++;
                }
                if (d1.debug)
                    printf("recv %s\n", recv);
            }
//      printf(" axis %d\n",axis);
            if (recv[0] != 'M' && recv[0] != 'T') {
                sprintf(txt, "comerr kk=%d n=%d mm%d", kk, n, count);
                iy = midy * 0.1;
                if (d1.displ) {
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                       iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_text(pixmap, fixed_font,
                                  drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                }
                d1.comerr++;
//          if (d1.fstatus == 1)
//            o.stroutfile(g, "* ERROR comerr");
                if (d1.mainten == 0)
                    d1.stow = 1;
                return;
            }
//        if (d1.azelsim != 0 && d1.azelsim < 10) sleep(1);
            sprintf(txt, "recv %s", str);
            iy = midy * 0.05;
            if (d1.displ) {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE,
                                   ix, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc,
                              ix, iy, txt, strlen(txt) - 1);
            }
            rcount = 0;
            sscanf(recv, "%*s %d", &rcount);
            if (rcount != count && ((axis == 0 && d1.azcmd != d1.azlim1)
                                    || (axis == 1 && d1.elcmd != d1.ellim1))) {
//      printf("lost count\n");

                sprintf(txt, "lost count goto Stow");
                d1.limiterr = 1;
                soutrack[0] = 0;
                iy = midy * 0.1;
                if (d1.displ) {
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                       iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_text(pixmap, fixed_font,
                                  drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                    sprintf(txt, "ERROR:  received %d counts out of %d counts expected", rcount, count);
                    iy = midy * 0.15;
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                       iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_text(pixmap, fixed_font,
                                  drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                    if (mm == 1)
                        sprintf(txt, "while going clockwise in azimuth");
                    if (mm == 0)
                        sprintf(txt, "while going counter-clockwise in azimuth");
                    if (mm == 3)
                        sprintf(txt, "while going clockwise in elevation");
                    if (mm == 2)
                        sprintf(txt, "while going counter-clockwise in elevation");
                    iy = midy * 0.20;
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                       iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_text(pixmap, fixed_font,
                                  drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                    sprintf(txt, "motor stalled or limit prematurely reached");
                    iy = midy * 0.25;
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                       iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_text(pixmap, fixed_font,
                                  drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                }
//          if (d1.fstatus == 1)
//            stroutfile("* ERROR lost count");
                if (d1.mainten == 0) {
                    if (mm == 2 && recv[0] == 'T') // could hit limit at source set
                    {
                        d1.elcount = 0;
                        d1.elnow = d1.ellim1;
                    }
                    elatstow = azatstow = 0;
                    d1.stow = 1;
                    d1.azcmd = d1.azlim1;
                    d1.elcmd = d1.ellim1;
                }
                return;
            }
            if (mm == 2 && recv[0] == 'T') {
                elatstow = 1;
                d1.elcount = 0;
                d1.elnow = d1.ellim1;
            }
            if (mm == 0 && recv[0] == 'T') {
                azatstow = 1;
                d1.azcount = 0;
                d1.aznow = d1.azlim1;
            }
            if (recv[0] == 'T' && d1.stow == 0) {
//          printf("timeout from antenna\n");
                sprintf(txt, "timeout from antenna");
                iy = midy * 0.1;
                if (d1.displ) {
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                                       iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_text(pixmap, fixed_font,
                                  drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
                }
            }
            if (recv[0] == 'M') {
                if (axis == 0) {
                    azatstow = 0;
                    if (mm == 1)
                        d1.azcount = d1.azcount + count;
                    else
                        d1.azcount = d1.azcount - count;
                }
                if (axis == 1) {
                    elatstow = 0;
                    if (mm == 3)
                        d1.elcount = d1.elcount + count;
                    else
                        d1.elcount = d1.elcount - count;
                }
            }
            if (d1.azelsim == 0 && d1.slew)
                sleep(1);
        }
        axis++;
        if (axis > 1)
            axis = 0;
    }
    if (d1.track != -1) {
        if (d1.slew == 1)
            d1.track = 0;
        else
            d1.track = 1;
    }
    d1.aznow = d1.azlim1 + d1.azcount / azscale;
    if (d1.aznow > 360.0)
        d1.aznow = d1.aznow - 360.0;
    if (!d1.rod)
        elnow = d1.elcount / elscale;
    else {
        elnow = -d1.elcount / d1.rod5 + lenzero;
        elnow = d1.rod1 * d1.rod1 + d1.rod2 * d1.rod2 - d1.rod3 * d1.rod3 - elnow * elnow;
        elnow = elnow / (2.0 * d1.rod1 * d1.rod2);
        elnow = -acos(elnow) * 180.0 / PI + d1.rod4 - d1.ellim1;
    }
    d1.elnow = d1.ellim1 + elnow;
    if (d1.elnow > 90.0) {
        if (d1.aznow >= 180.0)
            d1.aznow = d1.aznow - 180.0;
        else
            d1.aznow = d1.aznow + 180.0;
        d1.elnow = 180.0 - d1.elnow;
    }
//    d.dtext(670.0, 40.0, gg, Color.black,
//    "azel  " + d.dc(d1.aznow(), 5, 1) + " " + d.dc(d1.elnow(), 5, 1) +
//            " deg");
    x = (int) (d1.aznow * 640.0 / 360.0);
    if (d1.south == 0) {
        x -= 320;
        if (x < 0)
            x += 640;
    }
    if (fabs(d1.aznow - d1.azlim1) < 1e-6 && fabs(d1.elnow - d1.ellim1) < 1e-6) {
        if (d1.displ) {
            color.green = 0xffff;
            color.red = color.blue = 0;
            gdk_color_parse("green", &color);
            gtk_widget_modify_bg(button_stow, GTK_STATE_NORMAL, &color);
            gtk_tooltips_set_tip(tooltips, button_stow, "antenna at stow", NULL);
            gtk_tooltips_set_tip(tooltips, button_exit, "click to exit program", NULL);
//  printf("in green\n");  
        }
        d1.stow = -1;           // at stow

    } else {
        if (d1.displ) {
            color.green = color.red = color.blue = 0xffff;
            gtk_widget_modify_bg(button_stow, GTK_STATE_NORMAL, &color);
            gtk_tooltips_set_tip(tooltips, button_stow, "click to stow antenna", NULL);
            if (d1.track != -1)
                gtk_tooltips_set_tip(tooltips, button_exit, "go to stow first", NULL);
        }
        if (d1.stow == -1)
            d1.stow = 0;
    }
    if (d1.stow != 0) {
        d1.track = 0;
    }
    if (d1.displ) {
        sprintf(txt, "azel %5.1f %4.1f deg", d1.aznow, d1.elnow);
        iy = midy * 0.20;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        azel_to_radec(d1.secs, d1.aznow, d1.elnow, &ra, &dec);
        sprintf(txt, "ra %5.1f hr %4.1f deg", ra * 12.0 / PI, dec * 180.0 / PI);
        iy = midy * 0.30;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        sprintf(txt, "cmd  %5.1f %4.1f deg", d1.azcmd, d1.elcmd);
        iy = midy * 0.15;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
        sprintf(txt, "offsets %5.1f %4.1f deg", d1.azoff, d1.eloff);
        iy = midy * 0.25;
        gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
    } else if (d1.debug) {
        toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
        printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
        printf("now %s tant %5.1f Source: %s\n", txt, d1.tant, soutrack);
    }
    return;
}
