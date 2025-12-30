#include "header.h"
#include "prompt-1.h"
#include "command-2.h"

void sigchld_handler(int signum) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Minimal safe async output logic
        // (In production code, use write() not printf)
        if (WIFEXITED(status)) {
             char msg[1024];
             int len = snprintf(msg, sizeof(msg), "\nProcess with pid %d exited normally\n", pid);
             write(STDOUT_FILENO, msg, len);
        } else {
             char msg[1024];
             int len = snprintf(msg, sizeof(msg), "\nProcess with pid %d exited abnormally\n", pid);
             write(STDOUT_FILENO, msg, len);
        }
    }
}

int main() {
    char home_dir[PATH_MAX];
    if (getcwd(home_dir, sizeof(home_dir)) == NULL) {
        perror("Init error");
        return 1;
    }

    signal(SIGCHLD, sigchld_handler);

    char *input_buffer = NULL;
    size_t len = 0;

    while (1) {
        // Spec 1 Prompt
        display_prompt(home_dir);

        if (getline(&input_buffer, &len, stdin) == -1) {
            printf("\n");
            break;
        }

        // Spec 2 & 3 Process Input
        // Added home_dir argument for 'hop' resolution
        process_input(input_buffer, home_dir);
    }

    free(input_buffer);
    return 0;
}