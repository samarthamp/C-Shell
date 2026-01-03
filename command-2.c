#include "header.h"
#include "command-2.h"
#include "hop-3.h"
#include "reveal-4.h"
#include "log-5.h"
#include "proclore-7.h"
#include "seek-8.h"
#include "myshrc-9.h"

/* Define exactly once */
bg_job bg_jobs[MAX_BG];
int bg_count = 0;

void exec_command(char *cmd_str, int is_bg, char *home_dir, char *prev_dir) {
    // 1. Pre-parsing Check for Alias/Functions
    // We need to look at the first word without modifying cmd_str destructively yet.
    char temp_cmd[1024];
    strcpy(temp_cmd, cmd_str);
    char *first_word = strtok(temp_cmd, " \t\n");
    
    if (first_word == NULL) return;
    // --- CHECK ALIAS ---
    char *alias_val = get_alias(first_word);

    if (alias_val != NULL) {
        // Reconstruct command: alias_value + rest of args
        char new_cmd[1024];
        strcpy(new_cmd, alias_val);
        
        // Find end of first word in ORIGINAL string to get arguments
        char *args_ptr = cmd_str;
        while (*args_ptr == ' ' || *args_ptr == '\t') args_ptr++; // skip lead
        while (*args_ptr != ' ' && *args_ptr != '\t' && *args_ptr != '\0') args_ptr++; // skip word
        
        // Append arguments
        strcat(new_cmd, args_ptr);
        
        // RECURSE with new command string (allows alias -> built-in)
        exec_command(new_cmd, is_bg, home_dir, prev_dir);
        return;
    }

    // --- CHECK FUNCTION ---
    // Extract first argument for function (simple implementation supports $1)
    // We reuse temp_cmd tokenization
    char *func_arg = strtok(NULL, " \t\n"); // The second token
    
    if (execute_myshrc_function(first_word, func_arg, home_dir, prev_dir)) {
        return; // Function executed
    }

    // --- NORMAL EXECUTION (If not alias or function) ---

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