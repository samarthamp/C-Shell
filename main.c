#include "header.h"
#include "prompt-1.h"
#include "command-2.h"
#include "log-5.h" 
#include "myshrc-9.h"
#include "activities-13.h"
#include "signals-14.h"

void sigchld_handler(int signum) {
    int status;
    pid_t pid;

    // WNOHANG: Return immediately if no child has exited
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            // Check if process was in our background list
            // If remove_process returns 1, it was a tracked background process
            if (remove_process(pid)) {
                 char msg[1024];
                 int len = snprintf(msg, sizeof(msg), "\n%s with pid %d exited %s\n", 
                                    "Process", pid, WIFEXITED(status) ? "normally" : "abnormally");
                 write(STDOUT_FILENO, msg, len);
            }
        }
    }
}

int main() {
    char home_dir[PATH_MAX];
    char prev_dir[PATH_MAX] = "";

    if (getcwd(home_dir, sizeof(home_dir)) == NULL) {
        perror("Init error");
        return 1;
    }

    // --- SPEC 9: Load .myshrc ---
    load_myshrc(home_dir);

    // --- SETUP SIGNAL HANDLERS ---
    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, handle_sigint);   // Ctrl-C
    signal(SIGTSTP, handle_sigtstp); // Ctrl-Z

    char *input_buffer = NULL;
    size_t len = 0;

    while (1) {
        display_prompt(home_dir);

        if (getline(&input_buffer, &len, stdin) == -1) {
            // --- CTRL-D Handling ---
            // getline returns -1 on EOF (Ctrl-D)
            printf("\nLogging out...\n");
            
            // Kill all background processes before exiting
            kill_all_processes();
            break;
        }

        add_to_log(input_buffer, home_dir);
        process_input(input_buffer, home_dir, prev_dir);
    }

    free(input_buffer);
    return 0;
}