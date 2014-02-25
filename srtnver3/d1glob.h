/* globals */
#include <gtk/gtk.h>
extern double pwr, pwrst, pwrprev;
extern float spec[], avspec[], aavspec[], scanpwr[], avspecon[], avspecoff[], bspec[], bbspec[];
extern double pprev;
extern double ras[], decs[], epoc[];
extern int soutype[];
extern char sounam[NSOU][25];
extern char soutrack[25];
extern char souinfo[25];
extern d1type d1;
extern GdkPixmap *pixmap;
extern GdkPixmap *vpixmap;
extern GdkFont *fixed_font;
extern GdkFont *vfixed_font;
extern GtkWidget *entry1, *entry2, *entry3, *entry5, *entry6, *entry8;
extern GtkWidget *table;
extern GtkWidget *vtable;
extern GtkWidget *vwindow;
extern GtkWidget *drawing_area;
extern GtkWidget *vdrawing_area;
extern GtkWidget *button_stow, *button_record, *button_cmdfl, *button_npoint, *button_bsw, *button_exit, *button_cal;
extern GtkTooltips *tooltips;
extern int midx, midy;
