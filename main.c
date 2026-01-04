#include "header.h"
#include "prompt-1.h"
#include "command-2.h"
#include "log-5.h" 
#include "myshrc-9.h"
#include "activities-13.h"

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

    signal(SIGCHLD, sigchld_handler);

    char *input_buffer = NULL;
    size_t len = 0;

    while (1) {
        display_prompt(home_dir);

        if (getline(&input_buffer, &len, stdin) == -1) {
            printf("\n");
            break;
        }

        add_to_log(input_buffer, home_dir);
        process_input(input_buffer, home_dir, prev_dir);
    }

    free(input_buffer);
    return 0;
}