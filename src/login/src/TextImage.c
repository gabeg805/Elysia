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
//     Include the header file:
// 
//         #include "../hdr/TextImage.h"
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
//     init_text_root     - Initialize the root window
// 
//     draw_text          - Draw the password text
//     draw_text_window   - Draw the root window with all its objects
// 
//     display_text_image - Display the text image
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Text Image Window
//     * Draw Text Image
//     * Dislay Text Image
/// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated the header
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/TextImage.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>


// Declares
void init_text_root(GtkWidget *window, GtkWidget *area);
void draw_text(cairo_t *);
gboolean draw_text_window(GtkWidget *window);
void display_text_image();



// ////////////////////////////////////////
// ///// INITIALIZE TEXT IMAGE WINDOW /////
// ////////////////////////////////////////

// Initialize the root window and its objects
void init_text_root(GtkWidget *window, GtkWidget *area) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(window), TEXTIMAGE_XPOS, TEXTIMAGE_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), TEXTIMAGE_FSIZE*strlen(TEXTIMAGE_TEXT), TEXTIMAGE_FSIZE+1);
    
    // Add area to the root window
    gtk_container_add(GTK_CONTAINER(window), area);
    /* gtk_widget_set_app_paintable(window, TRUE); */
    
    // Attempt to enable window transparency
    enable_transparency(window);
    
    // GTK signals
    g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(draw_text_window), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);    
}



// ///////////////////////////
// ///// DRAW TEXT IMAGE /////
// ///////////////////////////

// Draw the text
void draw_text(cairo_t *cr) {         
    cairo_select_font_face(cr, TEXTIMAGE_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, TEXTIMAGE_FSIZE);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, 0, TEXTIMAGE_FSIZE);
    cairo_show_text(cr, TEXTIMAGE_TEXT);
}



// Draw the root window
gboolean draw_text_window(GtkWidget *window) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(window));
    
    // Check for window transparency
    if (supports_alpha) 
        cairo_set_source_rgba(cr, 0, 0, 0, 0); 
    else 
        cairo_set_source_rgb(cr, 1, 1, 1); 
    
    
    // Draw the window background 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
        
    // Draw the login frame
    draw_text(cr);
    cairo_destroy(cr);
    
    return FALSE;
}



// //////////////////////////////
// ///// DISPLAY TEXT IMAGE /////
// //////////////////////////////

// Dislay the text image
void display_text_image() {
    
    // Initialize text image elements
    GtkWidget *text_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *text = gtk_drawing_area_new();
    
    // Setup the text image
    init_text_root(text_window, text);
    
    // Display the password text image
    gtk_widget_show(text);
    gtk_widget_show(text_window);
}
