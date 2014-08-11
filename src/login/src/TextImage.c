// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     TextImage.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/TextImage.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o TextImage TextImage.c Transparency.c `pkg-config gtk+-3.0 --cflags --libs`
//         $ ./TextImage
// 
// 
// PURPOSE:
// 
//     Create and display the image of the word 'Password:'.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     init_text_root      - Initialize the root window
// 
//     draw_text           - Draw the password text
//     draw_text_window    - Draw the root window with all its objects
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Root Window
//     * Draw Text
/// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated the header
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/TextImage.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>

#define   XPOS    570
#define   YPOS    330
#define   NAME    "Password"
#define   FONT    "DejaVu Sans"
#define   FSIZE   12


// Declares
void init_text_root(GtkWidget *window, GtkWidget *area);
void draw_text(cairo_t *);
gboolean draw_text_window(GtkWidget *window);



// //////////////////////////////////
// ///// INITIALIZE ROOT WINDOW /////
// //////////////////////////////////

// Initialize the root window and its objects
void init_text_root(GtkWidget *window, GtkWidget *area) {
    
    // Set window attributes
    gtk_window_set_title(GTK_WINDOW(window), NAME);
    gtk_window_move(GTK_WINDOW(window), XPOS, YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), FSIZE*strlen(NAME), FSIZE+1);
    
    // Add area to the root window
    gtk_container_add(GTK_CONTAINER(window), area);
    gtk_widget_set_app_paintable(window, TRUE);
    
    // Attempt to enable window transparency
    enable_transparency(window);
    
    // GTK signals
    g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(draw_text_window), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);    
}



// /////////////////////
// ///// DRAW TEXT /////
// /////////////////////

// Draw the text
void draw_text(cairo_t *cr) {         
    
    // Text to display
    char text[strlen(NAME)+2];
    snprintf(text, sizeof(text), "%s: ", NAME);
    
    // Create the display box
    cairo_select_font_face (cr, FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, FSIZE);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_move_to (cr, 0, FSIZE);
    cairo_show_text (cr, text);
}



// Draw the root window
gboolean draw_text_window(GtkWidget *window) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(window));
    
    // Check for window transparency
    if (supports_alpha) 
        // Window transparent 
        cairo_set_source_rgba(cr, 0, 0, 0, 0); 
    else 
        // Window opaque white 
        cairo_set_source_rgb(cr, 1, 1, 1); 
    
    
    // Draw the window background 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    // Draw the login frame
    draw_text(cr);
    
    return FALSE;
}
