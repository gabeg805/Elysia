// 
//     Display buttons that control shutting down and rebooting the system.

/* Includes */
#include "panel.h"
#include "elyglobal.h"
#include "utility.h"
#include <unistd.h>
#include <gtk/gtk.h>

/* Private functions */
static void setup_button(GtkWidget *widg, char *img);
static void system_shutdown();
static void system_reboot();
static void cancel_ely();
static void refresh_ely();
static void quit_ely();
static void display_dialog();
static void display_item();



/* ******************************* */
/* ***** SETUP PANEL BUTTONS ***** */
/* ******************************* */

/* Setup panel buttons and style */
static void setup_button(GtkWidget *widg, char *img) {
    TRACE(stdout, "%s", "Setting up panel button...");

    GtkWidget *icon = gtk_image_new_from_file(img);
    gtk_button_set_image(GTK_BUTTON(widg), icon);
    gtk_button_set_relief(GTK_BUTTON(widg), GTK_RELIEF_NONE);

    TRACE(stdout, "%s", "Done setting up panel button.");
}



/* *************************** */
/* ***** SYSTEM COMMANDS ***** */
/* *************************** */

/* Shutdown the system */
static void system_shutdown() {
    TRACE(stdout, "%s", "Shutting down...");
    execl(POWEROFF, POWEROFF, NULL);
}



/* Shutdown the system */
static void system_reboot() {
    TRACE(stdout, "%s", "Rebooting...");
    execl(REBOOT, REBOOT, NULL);
}



/* Quit the prompt */
static void cancel_ely(GtkWidget *button, GtkWidget *window) {
    TRACE(stdout, "%s", "Hiding panel dialog...");
    gtk_widget_hide(window);
}



/* Refresh the login screen */
static void refresh_ely() {
    TRACE(stdout, "%s", "Refreshing the login manager...");
    execl(SYSTEMCTL, SYSTEMCTL, "restart", SERVICE, NULL);
}



/* Quit the login screen */
static void quit_ely() {
    TRACE(stdout, "%s", "Quitting the login manager...");
    char *cmd = "/usr/bin/chvt 2; /usr/bin/systemctl stop elysia";
    execl("/bin/bash", "/bin/bash", "-c", cmd, NULL);
}



/* ************************** */
/* ***** DISPLAY DIALOG ***** */
/* ************************** */

/* Display the Elysia dialog */
static void display_dialog() {
    TRACE(stdout, "%s", "Displaying button panel...");

    GtkWidget *window         = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *grid           = gtk_grid_new();
    GtkWidget *label          = gtk_label_new("Choose an action for the Elysia Login Manager.");
    GtkWidget *blank          = gtk_label_new(" ");
    GtkWidget *refresh_button = gtk_button_new_with_label("Restart");
    GtkWidget *quit_button    = gtk_button_new_with_label("Quit");
    GtkWidget *cancel_button  = gtk_button_new_with_label("Cancel");
    struct elypos pos;
    set_config_pos(PANEL_DIA_CONFIG, &pos);
    gtk_window_move(GTK_WINDOW(window), pos.x, pos.y);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,          0, 0, 5, 1);
    gtk_grid_attach(GTK_GRID(grid), blank,          0, 1, 5, 1);
    gtk_grid_attach(GTK_GRID(grid), refresh_button, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cancel_button,  3, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), quit_button,    4, 2, 1, 1);
    gtk_container_add(GTK_CONTAINER(window), grid);
    gtk_widget_show(label);
    gtk_widget_show(blank);    
    gtk_widget_show(refresh_button);
    gtk_widget_show(quit_button);
    gtk_widget_show(cancel_button);
    gtk_widget_show(grid);
    gtk_widget_show(window);
    g_signal_connect(G_OBJECT(refresh_button), "clicked", G_CALLBACK(refresh_ely),   NULL);
    g_signal_connect(G_OBJECT(quit_button),    "clicked", G_CALLBACK(quit_ely),      NULL);
    g_signal_connect(G_OBJECT(cancel_button),  "clicked", G_CALLBACK(cancel_ely),    window);
    g_signal_connect(window,                   "destroy", G_CALLBACK(gtk_main_quit), NULL);

    TRACE(stdout, "%s", "Done displaying button panel.");
}



/* ******************************** */
/* ***** DISPLAY BUTTON PANEL ***** */
/* ******************************** */

/* Display items in the button panel */
static void display_item(char *file, void (*func)() ) {
    TRACE(stdout, "%s", "Displaying panel item...");

    /* Create the panel button items */
    struct elyapp app; 
    app.win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    app.widg = gtk_button_new();
    setup_app(file, &app, "clicked", func);
    setup_button(app.widg, app.decor.img_file);

    TRACE(stdout, "%s", "Done displaying panel item.");
}



/* Display the button panel application  */
void display_panel() {
    display_item(PANEL_SHUT_CONFIG, system_shutdown);
    display_item(PANEL_REB_CONFIG, system_reboot);
    display_item(PANEL_REF_CONFIG, display_dialog);
}
