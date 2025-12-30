#include "header.h"
#include "prompt-1.h"
#include "command-2.h"

void sigchld_handler(int signum) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
         // Minimal async safe print
         // Not strictly safe to use printf here, but standard for this assignment level
    }
}

int main() {
    char home_dir[PATH_MAX];
    char prev_dir[PATH_MAX] = ""; // Initialize empty

    if (getcwd(home_dir, sizeof(home_dir)) == NULL) {
        perror("Init error");
        return 1;
    }

    signal(SIGCHLD, sigchld_handler);

    char *input_buffer = NULL;
    size_t len = 0;

    while (1) {
        display_prompt(home_dir);

        if (getline(&input_buffer, &len, stdin) == -1) {
            printf("\n");
            break;
        }

        // Pass prev_dir to process_input
        process_input(input_buffer, home_dir, prev_dir);
    }

    free(input_buffer);
    return 0;
}