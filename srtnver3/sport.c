#include <gtk/gtk.h>
#include <sys/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sched.h>
#include <fcntl.h>
#include <errno.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"



void azel(double az, double el)
  // command antenna movement
{
    int n, ix, iy, midxr, ixe, yr, da, hr, mn, sc;
    static int kk;
    double azz, ell, ra, dec, x, y;
    GdkColor color;
    char str[80], recv[256], txt[80];

    azz = ell = 0;
    d1.slew = 0;

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
    sprintf(txt, "cmd  %5.1f %4.1f deg", az, el);
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

    azz = az - d1.azlim1;
    ell = el - d1.ellim1;
    if (!d1.azelsim)
        d1.comerr = rot2(&azz, &ell, 1, recv); // initial read return if antenna at correct position
    else {
        azz = d1.azprev - d1.azlim1;
        ell = d1.elprev - d1.ellim1;
    }
    d1.aznow = azz + d1.azlim1;
    d1.elnow = ell + d1.ellim1;
    if (d1.debug)
        printf("aznow_after_read %3.0f elnow %3.0f\n", d1.aznow, d1.elnow);

    if ((fabs(d1.aznow - d1.azcmd) > 1.0 || fabs(d1.elnow - d1.elcmd) > 1.0)
        && (az >= d1.azlim1 && az < d1.azlim2 && el >= d1.ellim1 && el < d1.ellim2)) {
        azz = az - d1.azlim1;
        ell = el - d1.ellim1;
        if (!d1.azelsim) {
            d1.comerr = rot2(&azz, &ell, 2, recv); // command move
            d1.comerr = rot2(&azz, &ell, 1, recv); // read
        } else {
            azz = (d1.azcmd + d1.azprev) * 0.5 - d1.azlim1;
            ell = (d1.elcmd + d1.elprev) * 0.5 - d1.ellim1;
            if (fabs(d1.aznow - d1.azcmd) < 2.0)
                azz = d1.azcmd - d1.azlim1;
            if (fabs(d1.elnow - d1.elcmd) < 2.0)
                ell = d1.elcmd - d1.ellim1;
        }
        d1.azprev = d1.aznow = azz + d1.azlim1;
        d1.elprev = d1.elnow = ell + d1.ellim1;
        if (d1.debug)
            printf("aznow_after_cmd %3.0f elnow %3.0f cmd %3.0f %3.0f %3.0f %3.0f\n", d1.aznow, d1.elnow,
                   d1.azcmd, d1.elcmd, az, el);
    }

    if (d1.azelsim)
        sprintf(str, "antenna drive status:");
    else
        sprintf(str, "antenna drive simulated:");
    if (d1.comerr > 0) {
        sprintf(txt, " comerr= %d", d1.comerr);
        iy = midy * 0.1;
        if (d1.displ) {
            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                               iy - midy * 0.04, midxr, midy * 0.05);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
        }
    }
    if (az < d1.azlim1 || az > d1.azlim2 || el < d1.ellim1 || el > d1.ellim2) {
        sprintf(txt, "cmd out of limits");
        printf("cmd out of limits");
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
    if ((fabs(d1.aznow - d1.azcmd) > 1.5 || fabs(d1.elnow - d1.elcmd) > 1.5) && d1.track != -1) {
        if ((fabs(d1.aznow - d1.azcmd) > 1.5 || fabs(d1.elnow - d1.elcmd) > 1.5) && d1.stow != -1) {
            d1.slew = 1;
            sprintf(txt, "ant slewing");
            if (d1.printout) {
                toyrday(d1.secs, &yr, &da, &hr, &mn, &sc);
                printf("%4d:%03d:%02d:%02d:%02d %3s ", yr, da, hr, mn, sc, d1.timsource);
                if (soutrack[0]) {
                    printf("ant slewing to %s\n", soutrack);
                }
                if (d1.stow == 1) {
                    printf("ant slewing to stow\n");
                }
            }
            iy = midy * 0.1;
            if (d1.displ) {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc,
                                   TRUE, ix, iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
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
        n = 0;
        kk = 0;
        while (kk < 100 && n == 0) {
            azz = d1.aznow - d1.azlim1;
            ell = d1.elnow - d1.ellim1;
            if (!d1.azelsim)
                d1.comerr = rot2(&azz, &ell, 1, recv);
            else {
                azz = (d1.azcmd + d1.azprev) * 0.5 - d1.azlim1;
                ell = (d1.elcmd + d1.elprev) * 0.5 - d1.ellim1;
                if (fabs(d1.aznow - d1.azcmd) < 2.0)
                    azz = d1.azcmd - d1.azlim1;
                if (fabs(d1.elnow - d1.elcmd) < 2.0)
                    ell = d1.elcmd - d1.ellim1;
            }
            d1.azprev = d1.aznow = azz + d1.azlim1;
            d1.elprev = d1.elnow = ell + d1.ellim1;
            if (d1.printout)
                printf("aznow %3.0f elnow %3.0f k %d\n", d1.aznow, d1.elnow, kk);
            if (fabs(d1.aznow - d1.azcmd) > 1.0 || fabs(d1.elnow - d1.elcmd) > 1.0) {
                if (d1.printout)
                    printf("waiting on antenna cmd %3.0f %3.0f now %3.0f %3.0f kk %d\n", d1.azcmd, d1.elcmd,
                           d1.aznow, d1.elnow, kk);
                sprintf(txt, "waiting on antenna %d ", kk);
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
                    gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x - 4, y, x + 4, y);
                    gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_STATE_NORMAL], x, y - 4, x, y + 4);

                    iy = midy * 0.1;
//                        gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
//                                           iy - midy * 0.04, midxr, midy * 0.05);
                    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                                       iy - midy * 0.04, midxr, midy * 0.25);
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

//                            if (!d1.plot) {
                    Repaint();
//                            }

//                            cleararea();
                    while (gtk_events_pending() || d1.stopproc) {
                        gtk_main_iteration();
//                                 d1.plot = 0;
                    }

                } else if (d1.debug)
                    printf("%s\n", txt);
                sleep(1);
            } else
                n = 1;
            kk++;
        }
        if (d1.debug)
            printf("recv %s\n", recv);
        if (d1.comerr) {
            sprintf(txt, "comerr %d", d1.comerr);
            iy = midy * 0.1;
            if (d1.displ) {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                   iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
            }
            if (d1.mainten == 0)
                d1.stow = 1;
            return;
        }
        sprintf(txt, "recv %s", recv);
        iy = midy * 0.05;
        if (d1.displ) {
            gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE,
                               ix, iy - midy * 0.04, midxr, midy * 0.05);
            gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt) - 1);
        }
        if (fabs(d1.aznow - d1.azcmd) > 1.0 || fabs(d1.elnow - d1.elcmd) > 1.0) {
//      printf("lost count\n");

            sprintf(txt, "lost count goto Stow");
            d1.limiterr = 1;
            soutrack[0] = 0;
            iy = midy * 0.1;
            if (d1.displ) {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                   iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                sprintf(txt, "ERROR:  stopped at %3.0f %2.0f", d1.aznow, d1.elnow);
                iy = midy * 0.15;
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                   iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                sprintf(txt, "while slewing");
                iy = midy * 0.20;
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                   iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
                sprintf(txt, "motor stalled or limit prematurely reached");
                iy = midy * 0.25;
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ixe,
                                   iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ixe, iy, txt, strlen(txt));
            }
            if (d1.mainten == 0) {
                if (d1.azcmd < d1.azlim1 + 1 || d1.elcmd < d1.ellim1 + 1) // could hit limit at source set
                {
                    d1.elnow = d1.ellim1;
                }
                d1.stow = 1;
                d1.azcmd = d1.azlim1;
                d1.elcmd = d1.ellim1;
            }
            return;
        }
        if (d1.comerr == -1) {
//          printf("timeout from antenna\n");
            sprintf(txt, "timeout from antenna");
            iy = midy * 0.1;
            if (d1.displ) {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, ix,
                                   iy - midy * 0.04, midxr, midy * 0.05);
                gdk_draw_text(pixmap, fixed_font, drawing_area->style->black_gc, ix, iy, txt, strlen(txt));
            }
        }
        if (d1.comerr == 0) {   // normal response
        }
        if (d1.slew)
            sleep(1);
    }
    if (d1.track != -1) {
        if (d1.slew == 1)
            d1.track = 0;
        else
            d1.track = 1;
    }
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


int rot2(double *az, double *el, int cmd, char *resp)
{
    int usbdev, status, i, rstatus;
    char command[13];
// for perm add to dialout group
    i = rstatus = 0;
//  system("stty -F /dev/ttyUSB0 600 cs8 -cstopb -parenb -icanon min 1 time 1");
    if (cmd == -1) {
        system("stty -F /dev/ttyUSB0 600 raw -echo time 2");
        return 0;
    }
    usbdev = 0;                 // cmd  0x0f=stop 0x1f=status 0x2f=set
    usbdev = open("/dev/ttyUSB0", O_RDWR, O_NONBLOCK);
//  printf("usbdev %d\n",usbdev);
    cmd = cmd * 16 + 0xf;
    sprintf(command, "W%04d%c%04d%c%c ", (int) *az + 360, 1, (int) *el + 360, 1, cmd);
//  for(i=0;i<13;i++) printf("isend=%d ch=%2x\n",i,command[i]);
    status = write(usbdev, command, 13);
//  printf("write status %d\n",status);
    sleep(1);
    if (cmd == 0x1f) {
        rstatus = read(usbdev, resp, 12);
//  printf("read status %d\n",rstatus);
//  for(i=0;i<12;i++) printf("irec=%d ch=%2x\n",i,resp[i]);
        *az = resp[1] * 100 + resp[2] * 10 + resp[3] - 360;
        *el = resp[6] * 100 + resp[7] * 10 + resp[8] - 360;
    }
    close(usbdev);
    if (rstatus != 12)
        return -1;
    else
        return 0;
}
