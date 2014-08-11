// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     glm.c
// 
// 
// SYNTAX: 
// 
//     ./glm
// 
// 
// PURPOSE:
// 
//     Display "Gabe's Login Manager", a login manager I created that is extremely 
//     customizable, where any item(s) can be added/removed/modified on the login 
//     screen.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     main - Display Gabe's Login Manager
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Gabe's Login Manager
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 07 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated the header
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Username.h"
#include "../hdr/Password.h"
#include "../hdr/Interface.h"
#include "../hdr/Authenticate.h"
#include "../hdr/CommandLine.h"
#include "../hdr/OpenDisplay.h"

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <paths.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define XTTY           "tty7"
#define log            "/etc/X11/glm/log/interface.log"
#define flag           "TRUE"



// ////////////////////////////////
// ///// GABE'S LOGIN MANAGER ///// 
// ////////////////////////////////

// Display Gabe's Login Manager 
int main(int argc, char *argv[]) {
    
    // Set the display environment variable
    char *DISPLAY = get_open_display();
    setenv("DISPLAY", DISPLAY, 1);
    
    
    // Setup the X server for logging in
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        char *xsetup = "/etc/X11/glm/src/x/Xsetup";
        execlp(xsetup, xsetup, DISPLAY, XTTY, NULL);
    } else {
        int status;
        waitpid(child_pid, &status, 0);
    }
    
    
    // Log to file that interface is beginning execution
    FILE *fp;
    fp = fopen(log, "w");
    fprintf(fp, "%s\n", "FALSE");
    fclose(fp);
    
    
    // Display login interface
    if ( !fork() ) {
        login_interface(argc, argv);
        return 0;
    }
    
    
    // Login to the system
    int status = 1;
    while (status) {
        char temp[1024];
        char input[1024];
        
        fp = fopen(log, "r");
        strcpy(input, "");
        while (fgets(temp, sizeof(input)-1, fp) != NULL ) {        
            strcat(input, temp);
        }
        
        // Remove trailing newline characters
        input[strlen(input)-1] = '\0';
        fclose(fp);
        
        if ( strcmp(input, flag) == 0 ) {
            while (status) {
                char *PASSWORD = password_entry(argc, argv);
                
                FILE *handle = fopen("/etc/X11/glm/log/user.log", "r");
                char temp[1024];
                fgets(temp, sizeof(temp), handle);
                char USERNAME[strlen(temp)];
                strcpy(USERNAME, "");
                strncat(USERNAME, temp, strlen(temp)); 
                fclose(handle);
                
                status  = login(USERNAME, PASSWORD);
                free(PASSWORD);
            }
        }
    }
    
    return 0;
}
