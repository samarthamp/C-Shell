#include "header.h"
#include "prompt-1.h"
#include "command-2.h"

// Signal handler for SIGCHLD
void sigchld_handler(int signum) {
    int status;
    pid_t pid;

    // waitpid with WNOHANG checks if any child has exited without blocking.
    // We loop because multiple children might exit simultaneously.
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            // Write to stderr or stdout. 
            // Safe way involves write() and avoiding buffered IO.
            char msg[1024];
            int len = snprintf(msg, sizeof(msg), "\n%s with pid %d exited normally\n", "Process", pid);
            write(STDOUT_FILENO, msg, len);
            
            // Re-prompt is tricky in signal handlers, usually skipped or handled by flags.
            // We will just print the message.
        } else {
            char msg[1024];
            int len = snprintf(msg, sizeof(msg), "\n%s with pid %d exited abnormally\n", "Process", pid);
            write(STDOUT_FILENO, msg, len);
        }
    }
}

int main() {
    char home_dir[PATH_MAX];
    if (getcwd(home_dir, sizeof(home_dir)) == NULL) {
        perror("Failed to initialize shell home directory");
        return 1;
    }

    // Register Signal Handler
    signal(SIGCHLD, sigchld_handler);

    char *input_buffer = NULL;
    size_t len = 0;

    while (1) {
        display_prompt(home_dir);

        if (getline(&input_buffer, &len, stdin) == -1) {
            printf("\n");
            break;
        }

        // Process the input (Specification 2)
        process_input(input_buffer);
    }

    free(input_buffer);
    return 0;
}