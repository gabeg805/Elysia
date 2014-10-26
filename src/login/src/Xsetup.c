// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Xsetup.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "../hdr/Xsetup.h"
// 
// 
// PURPOSE:
// 
//     Sets up an X server along side a compositing manager to enable transparency.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     start_xserver - Start the X server
// 
//     start_compman - Start the compositing manager
// 
//     xsetup        - Setup X 
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Start X Server
//     * Start Compositing Manager
//     * Setup X 
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 17 2014 <> Created.
// 
//     gabeg Aug 18 2014 <> Added an algorithm to time the start of the compositing 
//                          manager more approriately.
// 	
//     gabeg Sep 16 2014 <> Removed unneeded libraries.
// 
//     gabeg Oct 25 2014 <> Changed "start_xserver" and "start_compman" functions to 
//                          static and trimmed down the header file so it does not 
//                          contain unnecessary includes. 
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/Xsetup.h"
#include "../hdr/Utility.h"
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DATE           "/usr/bin/date"
#define HSETROOT       "/usr/bin/hsetroot"
#define TAIL           "/usr/bin/tail"
#define WALLPAPER      "/etc/X11/glm/img/wallpapers/night-sky.jpg"
#define XCOMPMGR       "/usr/bin/xcompmgr"
#define XORG           "/usr/bin/Xorg"
#define XSERVER_AUTH   "/etc/X11/glm/log/glm.auth"
#define XSERVER_LOG    "/etc/X11/glm/log/xserver.log"
#define XSETROOT       "/usr/bin/xsetroot"



// Declares
static void start_xserver();
static void start_compman();
void xsetup();



// //////////////////////////
// ///// START X SERVER /////
// //////////////////////////

// Start the X server
void start_xserver() {
    
    // Log X start
    file_write(GLM_LOG, "a+", "%s\n", "Starting X server...");
    
    
    // Set the display environment variable
    DISPLAY = get_open_display();
    setenv("DISPLAY", DISPLAY, 1);
    
    // Get open tty port
    TTY_N = get_open_tty();
    char VT[6];
    snprintf(VT, sizeof(VT), "%s%d", "vt", TTY_N);
    
    // Start X server
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        execl(XORG, XORG, "-logverbose", "-logfile", XSERVER_LOG, 
              "-nolisten", "tcp", DISPLAY, "-auth",
              XSERVER_AUTH, VT, NULL);
    }
    
        
    // Log X start
    file_write(GLM_LOG, "a+", "%s\n", "X server is active.");
    
    
    // Free display memory
    free(DISPLAY);
    DISPLAY = NULL;
}



// /////////////////////////////////////
// ///// START COMPOSITING MANAGER /////
// /////////////////////////////////////

// Start compositing manager (for transparency)
void start_compman() {
    
    // Log compositing manager start
    file_write(GLM_LOG, "a+", "%s\n", "Starting compositing manager...");    
    
    // Check if composite manager is already running
    char val[5];
    FILE *handle = popen("pgrep -c xcompmgr", "r");
    fgets(val, sizeof(val), handle);
    pclose(handle);
    
    if ( atoi(val) != 0 )
        return;
    
    // Initialize monotonic clock
    struct timespec start, end; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Time the logging to X server log file
    long BILLION = 1000000000L;
    int count = 0;
    char *last = NULL;
    int i = 0;
    while (1) {
        
        // Calculate time between loops
        clock_gettime(CLOCK_MONOTONIC, &end); 
        int64_t diff  = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec; 
        
        // Check if server file exists
        if ( access(XSERVER_LOG, F_OK) != 0 )
            continue;
        
        // Get the last line of X server log file
        char cmd[100];        
        snprintf(cmd, sizeof(cmd), "%s %s %s", TAIL, "-1", XSERVER_LOG);
        char **xcheck = command_line(cmd, sizeof(cmd));
        int num = atoi(xcheck[0]);
        
        // Make sure file is not empty
        if ( num == 1 ) {
            
            // Increment count and free last
            if ( last != NULL ) {
                if ( strcmp(last, xcheck[1]) == 0 ) 
                    ++count;
                free(last);
            }
            
            // Define last
            last = strdup(xcheck[1]);
            
            // Free memory
            free(xcheck[1]);
            xcheck[1] = NULL;
        }
        
        // Free memory
        free(xcheck[0]);
        xcheck[0] = NULL;
        
        free(xcheck);
        xcheck = NULL;
        
        
        // Once a safe amount of time has elapsed, execute the compositing manager
        if ( (PREVIEW) || (count == 200) || (diff >= 5*BILLION) ) {
            pid_t new_pid = fork();
            if ( new_pid == 0 )
                execl(XCOMPMGR, XCOMPMGR, NULL);
            break;
        }
        ++i;
    }
    
    
    // Log status of composite manager
    file_write(GLM_LOG, "a+", "%s\n", "Composite manager is active.");
}



// ///////////////////
// ///// SETUP X /////
// ///////////////////
 
// Setup X for login manager
void xsetup() {
    
    // Log program start
    char **date_str = command_line(DATE, 40);
    file_write(GLM_LOG, "a+", "\n%s %s\n%s %d\n\n", "Date:", date_str[1], "Preview:", PREVIEW);
    
    // Start X server when not in preview mode
    if ( !PREVIEW ) 
        start_xserver();
    
    // Start compositing manager
    start_compman();        
    
    // Set background 
    char hcmd[75];
    snprintf(hcmd, sizeof(hcmd), "%s %s %s", HSETROOT, "-fill", WALLPAPER);
    system(hcmd);
    
    // Cursor and background attributes
    char xcmd[50];
    snprintf(xcmd, sizeof(xcmd), "%s %s %s", XSETROOT, "-cursor_name", "left_ptr");
    system(xcmd);
    
    // Log that interface is allowed start
    INTERFACE = 1;
    
    // Free memory
    free(date_str[1]);
    date_str[1] = NULL;

    free(date_str[0]);
    date_str[0] = NULL;    
    
    free(date_str);
    date_str = NULL;
}
