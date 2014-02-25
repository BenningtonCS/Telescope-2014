#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sched.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"
#include "hproto.h"

/* srthelp.c
 * June 9, 2005
 * by Eric Evarts
 * 
 * This file is for the graphical interface for the help features of the SRT
 * program.  The basic starting place for this help setup is the original
 * Java code written for the SRT.
 *
 * Display interface uses GTK+-2.0
 */

GtkWidget *textbox;
GtkWidget *helpwindow;

// display_help will open a new window for the help to be displayed
void display_help()
{
    PangoFontDescription *font_desc;
    GtkWidget *vertbox, *buttonbox;
    GtkWidget *scrollbox;
    GtkWidget *button_helphome, *button_helpmode, *button_helpnpoint, *button_helpbeamsw,
        *button_helpfreq, *button_helpoffset, *button_helprecord, *button_helpcmdfl,
        *button_helpcal, *button_helpexit;
    GdkGeometry geometry;
    GdkWindowHints geo_mask;
    //  GdkRectangle update_rect;
    GdkColor color;

    if (d1.helpwindow) {
        gtk_widget_destroy(helpwindow);
        d1.helpwindow = 0;
    }
    // Create the new window and set up some specifics
    helpwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    d1.helpwindow = 1;
    // set the title
    gtk_window_set_title(GTK_WINDOW(helpwindow), "SRT Help");
    // Set the geometry of the window
    geometry.min_width = 300;
    geometry.min_height = 300;
    geo_mask = GDK_HINT_MIN_SIZE;
    gtk_window_set_geometry_hints(GTK_WINDOW(helpwindow), helpwindow, &geometry, geo_mask);
    // set default size
    gtk_window_set_default_size(GTK_WINDOW(helpwindow), 500, 500);
    // set the color
    color.red = 0xffff;
    color.blue = 0xffff;
    color.green = 0xffff;

    // Set up the way to destroy this window
    g_signal_connect(G_OBJECT(helpwindow), "destroy", G_CALLBACK(exit_help), NULL);
    g_signal_connect(G_OBJECT(helpwindow), "delete_event", G_CALLBACK(exit_help), NULL);

    // Create a vertical box to hold the button box and the text view
    // FALSE indicates not homogenous, 0 indicates no extra spacing
    vertbox = gtk_vbox_new(FALSE, 0);

    // Create the button box as a horizontal box with not homogenous sizes
    buttonbox = gtk_hbox_new(FALSE, 0);

    // Create the buttons to populate the button box
    button_helphome = gtk_button_new_with_label("Home");
    button_helpmode = gtk_button_new_with_label("Mode");
    button_helpnpoint = gtk_button_new_with_label("NPoint");
    button_helpbeamsw = gtk_button_new_with_label("BeamSW");
    button_helpfreq = gtk_button_new_with_label("Freq");
    button_helpoffset = gtk_button_new_with_label("Offset");
    button_helprecord = gtk_button_new_with_label("Record");
    button_helpcmdfl = gtk_button_new_with_label("CmdFl");
    button_helpcal = gtk_button_new_with_label("Cal");
    button_helpexit = gtk_button_new_with_label("Exit Help");

    // Populate the button box and show the buttons
    // gtk_box_pack_start(box, widget to pack, expand?, fill?, padding)
    // gtk_widget_show(widget to pack)
    // g_signal_connect(G_OBJECT(widget to pack), "command",
    //                  G_CALLBACK(command_function), NULL)
    gtk_box_pack_start(GTK_BOX(buttonbox), button_helphome, TRUE, TRUE, 0);
    gtk_widget_show(button_helphome);
    g_signal_connect(G_OBJECT(button_helphome), "clicked", G_CALLBACK(button_helphome_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(buttonbox), button_helpmode, TRUE, TRUE, 0);
    gtk_widget_show(button_helpmode);
    g_signal_connect(G_OBJECT(button_helpmode), "clicked", G_CALLBACK(button_helpmode_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(buttonbox), button_helpnpoint, TRUE, TRUE, 0);
    gtk_widget_show(button_helpnpoint);
    g_signal_connect(G_OBJECT(button_helpnpoint), "clicked", G_CALLBACK(button_helpnpoint_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(buttonbox), button_helpbeamsw, TRUE, TRUE, 0);
    gtk_widget_show(button_helpbeamsw);
    g_signal_connect(G_OBJECT(button_helpbeamsw), "clicked", G_CALLBACK(button_helpbeamsw_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(buttonbox), button_helpfreq, TRUE, TRUE, 0);
    gtk_widget_show(button_helpfreq);
    g_signal_connect(G_OBJECT(button_helpfreq), "clicked", G_CALLBACK(button_helpfreq_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(buttonbox), button_helpoffset, TRUE, TRUE, 0);
    gtk_widget_show(button_helpoffset);
    g_signal_connect(G_OBJECT(button_helpoffset), "clicked", G_CALLBACK(button_helpoffset_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(buttonbox), button_helprecord, TRUE, TRUE, 0);
    gtk_widget_show(button_helprecord);
    g_signal_connect(G_OBJECT(button_helprecord), "clicked", G_CALLBACK(button_helprecord_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(buttonbox), button_helpcmdfl, TRUE, TRUE, 0);
    gtk_widget_show(button_helpcmdfl);
    g_signal_connect(G_OBJECT(button_helpcmdfl), "clicked", G_CALLBACK(button_helpcmdfl_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(buttonbox), button_helpcal, TRUE, TRUE, 0);
    gtk_widget_show(button_helpcal);
    g_signal_connect(G_OBJECT(button_helpcal), "clicked", G_CALLBACK(button_helpcal_clicked), NULL);

    // Help exit is a special case that swaps control to the destroy window  
    gtk_box_pack_start(GTK_BOX(buttonbox), button_helpexit, TRUE, TRUE, 0);
    gtk_widget_show(button_helpexit);
    g_signal_connect_swapped(G_OBJECT(button_helpexit), "clicked",
                             G_CALLBACK(gtk_widget_destroy), G_OBJECT(helpwindow));

    // put the button box into the vertical box and show it
    gtk_box_pack_start(GTK_BOX(vertbox), buttonbox, FALSE, FALSE, 0);
    gtk_widget_show(buttonbox);

    // create the textview
    // first make the scrollwindow to put it in
    scrollbox = gtk_scrolled_window_new(NULL, NULL);
    // set the policy so that there is only a vertical scroll bar if needed
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbox), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    // actually create text box and set some policy for it
    textbox = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textbox), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textbox), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textbox), GTK_WRAP_WORD);
    font_desc = pango_font_description_from_string("Sans 15");
    gtk_widget_modify_font(textbox, font_desc);
    pango_font_description_free(font_desc);
    // Initialize the textview with the home data
    button_helphome_clicked();
    //put the textbox into the scrolled window
    gtk_container_add(GTK_CONTAINER(scrollbox), textbox);
    gtk_widget_show(scrollbox);

    // put the text view into the vertical box and show it
    gtk_box_pack_start(GTK_BOX(vertbox), scrollbox, TRUE, TRUE, 0);
    gtk_widget_show(textbox);

    // put the vert box into the window
    gtk_container_add(GTK_CONTAINER(helpwindow), vertbox);
    gtk_widget_show(vertbox);

    // Show the help window now that it is complete
    gtk_widget_show(helpwindow);

}


void button_helphome_clicked(void)
{
    load_help("home");
}

void button_helpmode_clicked(void)
{
    load_help("mode");
}

void button_helpnpoint_clicked(void)
{
    load_help("npoint");
}

void button_helpbeamsw_clicked(void)
{
    load_help("beamsw");
}

void button_helpfreq_clicked(void)
{
    load_help("freq");
}

void button_helpoffset_clicked(void)
{
    load_help("offset");
}

void button_helprecord_clicked(void)
{
    load_help("record");
}

void button_helpcmdfl_clicked(void)
{
    load_help("cmdfl");
}

void button_helpcal_clicked(void)
{
    load_help("cal");
}

void button_helpexit_clicked(void)
{
    GtkTextBuffer *buffer;
    char text[256];

    // get the current buffer
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));

    // set the text
    sprintf(text, "Exit");
    gtk_text_buffer_set_text(buffer, text, -1);
    d1.helpwindow = 0;

}

void load_help(char *command)
{
    FILE *file1;
    GtkTextBuffer *buffer;
    char text[256];
    char cmd[15] = "##";
    // add command to the end of cmd to get the string we are looking for
    strcat(cmd, command);
    // Open the help source file
    if ((file1 = fopen(d1.hlpnam, "r")) == NULL) {
        printf("Unable to open %s\n", d1.hlpnam);
        return;
    }
    // get a new buffer
    buffer = gtk_text_buffer_new(NULL);

    // find the starting location of this command in the file (##command)
    while (strstr(text, cmd) == NULL) {
        if (fgets(text, 256, file1) == NULL) {
            printf("cannot find help file content\n");
            return;
        }
    }

    // get the first line to output
    fgets(text, 256, file1);
    // now loop until we find a line starting with ## to print text
    while ((text[0] != '#') && (text[1] != '#')) {
        gtk_text_buffer_insert_at_cursor(buffer, text, -1);
        fgets(text, 256, file1);
    }

    // Set the textbox to display the new buffer
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(textbox), buffer);

    //close the file
    fclose(file1);
}

// process the exit command
void exit_help(void)
{
    d1.helpwindow = 0;
}
