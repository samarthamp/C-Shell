#include "header.h"
#include "command-2.h"
#include "hop-3.h"
#include "reveal-4.h"

void exec_command(char *cmd_str, int is_bg, char *home_dir, char *prev_dir) {
    char *argv[64];
    int argc = 0;

    char *token = strtok(cmd_str, " \t\n");
    while (token != NULL && argc < 63) {
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;

    if (argc == 0) return;

    // --- BUILT-INS ---
    if (strcmp(argv[0], "hop") == 0) {
        execute_hop(argv, home_dir, prev_dir);
        return;
    }
    if (strcmp(argv[0], "reveal") == 0) {
        execute_reveal(argv, home_dir, prev_dir);
        return;
    }

    // --- EXTERNAL ---
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return;
    }
    if (pid == 0) {
        if (is_bg) setpgid(0, 0);
        if (execvp(argv[0], argv) == -1) {
            printf("ERROR: '%s' is not a valid command\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        if (is_bg) {
            printf("[%d] %d\n", 1, pid);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void process_input(char *input, char *home_dir, char *prev_dir) {
    char *ptr = input;
    char *start = input;
    while (*ptr != '\0') {
        if (*ptr == ';' || *ptr == '&') {
            int is_bg = (*ptr == '&');
            *ptr = '\0';
            exec_command(start, is_bg, home_dir, prev_dir);
            start = ptr + 1;
        }
        ptr++;
    }
    if (*start != '\0') {
        exec_command(start, 0, home_dir, prev_dir);
    }
}