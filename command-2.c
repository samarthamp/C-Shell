#include "header.h"
#include "command-2.h"

// Helper to execute a single command
void exec_command(char *cmd_str, int is_bg) {
    // 1. Tokenize the command string into arguments
    // We use a temporary array for argv. Size 64 is usually sufficient for basic shells.
    char *argv[64];
    int argc = 0;

    char *token = strtok(cmd_str, " \t\n");
    while (token != NULL && argc < 63) {
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL; // Null-terminate the list

    // If command is empty (e.g., user typed ";;"), return
    if (argc == 0) return;

    // 2. Fork and Execute
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return;
    }

    if (pid == 0) {
        // --- CHILD PROCESS ---
        
        // If background process, we set the process group (optional but good practice)
        if (is_bg) {
            setpgid(0, 0);
        }

        // Execute command
        if (execvp(argv[0], argv) == -1) {
            printf("ERROR: '%s' is not a valid command\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        // --- PARENT PROCESS ---
        if (is_bg) {
            // Background: Print PID and return immediately
            // Note: Job ID [1] is hardcoded for now or needs a global counter. 
            // We use the PID as the unique identifier.
            printf("[%d] %d\n", 1, pid); 
        } else {
            // Foreground: Wait for the specific child to finish
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void process_input(char *input) {
    // We need to split by ';' and '&'. 
    // strtok is risky here because we need to know WHICH delimiter terminated the token
    // to decide if it's background or foreground.
    
    char *ptr = input;
    char *start = input;
    
    // Iterate through the string
    while (*ptr != '\0') {
        if (*ptr == ';' || *ptr == '&') {
            int is_bg = (*ptr == '&');
            
            // Replace delimiter with null terminator to isolate the command
            *ptr = '\0';
            
            // Execute the isolated command
            exec_command(start, is_bg);
            
            // Move start to the next character
            start = ptr + 1;
        }
        ptr++;
    }
    
    // Execute the final command (if any)
    // The last command is always foreground unless it ended with '&' which we caught in the loop
    // Actually, if the line ends with "sleep 5 &", the loop catches the '&'.
    // If it ends with "ls", the loop finishes and we have "ls" left in 'start'.
    if (*start != '\0') {
        exec_command(start, 0); // Default to foreground for last command
    }
}