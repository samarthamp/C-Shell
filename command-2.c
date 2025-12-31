#include "header.h"
#include "command-2.h"
#include "hop-3.h"
#include "reveal-4.h"
#include "log-5.h"
#include "proclore-7.h"
#include "seek-8.h"

/* Define exactly once */
bg_job bg_jobs[MAX_BG];
int bg_count = 0;

void exec_command(char *cmd_str, int is_bg, char *home_dir, char *prev_dir) {
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

    // --- BUILT-INS ---
    if (strcmp(argv[0], "hop") == 0) {
        execute_hop(argv, home_dir, prev_dir);
        return;
    }
    if (strcmp(argv[0], "reveal") == 0) {
        execute_reveal(argv, home_dir, prev_dir);
        return;
    }
    // Log command
    if (strcmp(argv[0], "log") == 0) {
        execute_log(argv, home_dir, prev_dir);
        return;
    }

    // Proclore command
    if (strcmp(argv[0], "proclore") == 0) {
        execute_proclore(argv, home_dir);
        return;
    }

    // Seek command
    if (strcmp(argv[0], "seek") == 0) {
        execute_seek(argv, home_dir, prev_dir);
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
            bg_jobs[bg_count].pid = pid;
            strcpy(bg_jobs[bg_count].name, argv[0]); // command name
            bg_count++;
            printf("[%d] %d\n", 1, pid);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void process_input(char *input, char *home_dir, char *prev_dir) {
    // Standard parsing loop (handles ; and &)
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