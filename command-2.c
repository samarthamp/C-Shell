#include "header.h"
#include "command-2.h"
#include "hop-3.h" 

// Helper to execute a single command
void exec_command(char *cmd_str, int is_bg, char *home_dir) {
    char *argv[64];
    int argc = 0;

    // Tokenize
    char *token = strtok(cmd_str, " \t\n");
    while (token != NULL && argc < 63) {
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;

    if (argc == 0) return;

    // --- CHECK FOR BUILT-IN COMMANDS ---
    // If command is "hop", run it in the PARENT process.
    if (strcmp(argv[0], "hop") == 0) {
        execute_hop(argv, home_dir);
        return; // Do not fork
    }

    // --- EXTERNAL COMMANDS ---
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return;
    }

    if (pid == 0) {
        // CHILD
        if (is_bg) setpgid(0, 0);

        if (execvp(argv[0], argv) == -1) {
            printf("ERROR: '%s' is not a valid command\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        // PARENT
        if (is_bg) {
            printf("[%d] %d\n", 1, pid);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void process_input(char *input, char *home_dir) {
    char *ptr = input;
    char *start = input;
    
    while (*ptr != '\0') {
        if (*ptr == ';' || *ptr == '&') {
            int is_bg = (*ptr == '&');
            *ptr = '\0';
            exec_command(start, is_bg, home_dir);
            start = ptr + 1;
        }
        ptr++;
    }
    
    if (*start != '\0') {
        exec_command(start, 0, home_dir);
    }
}