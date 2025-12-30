#include "header.h"
#include "hop-3.h"

void execute_hop(char **args, char *home_dir) {
    // Static variable to persist the previous directory across function calls
    static char prev_dir[PATH_MAX] = "";
    
    // Get current directory to update prev_dir later if successful
    char current_cwd[PATH_MAX];
    if (getcwd(current_cwd, sizeof(current_cwd)) == NULL) {
        perror("hop: getcwd failed");
        return;
    }

    // If prev_dir is empty (first run), initialize it to current directory
    if (strlen(prev_dir) == 0) {
        strcpy(prev_dir, current_cwd);
    }

    // CASE 1: No arguments -> hop to home
    if (args[1] == NULL) {
        // We treat this like "hop ~"
        // We construct a temporary arg list to reuse logic or just do it here.
        // Doing it here for clarity.
        if (chdir(home_dir) != 0) {
            perror("hop");
        } else {
            strcpy(prev_dir, current_cwd); // Update previous to where we came from
            printf("%s\n", home_dir);
        }
        return;
    }

    // CASE 2: Multiple arguments -> Sequential execution
    int i = 1;
    while (args[i] != NULL) {
        char target_path[PATH_MAX];
        char *arg = args[i];
        int success = 0;

        // --- Resolve Target Path ---
        
        if (strcmp(arg, "~") == 0) {
            strcpy(target_path, home_dir);
        } 
        else if (strcmp(arg, "-") == 0) {
            if (strlen(prev_dir) == 0) {
                printf("hop: OLDPWD not set\n");
                i++;
                continue;
            }
            strcpy(target_path, prev_dir);
        } 
        else if (strncmp(arg, "~/", 2) == 0) {
            // Replaces ~/ with home_dir/
            snprintf(target_path, PATH_MAX, "%s/%s", home_dir, arg + 2);
        } 
        else {
            // Absolute path, relative path, ., .. are all handled natively by chdir
            strcpy(target_path, arg);
        }

        // --- Execute Change ---
        
        // Save current location before moving, so we can update prev_dir
        char temp_current[PATH_MAX];
        getcwd(temp_current, sizeof(temp_current));

        if (chdir(target_path) != 0) {
            perror("hop");
        } else {
            // Success: Update prev_dir to where we were BEFORE this specific hop
            strcpy(prev_dir, temp_current);
            
            // Print new absolute path
            char new_cwd[PATH_MAX];
            if (getcwd(new_cwd, sizeof(new_cwd)) != NULL) {
                printf("%s\n", new_cwd);
            }
        }

        i++;
    }
}