#include "header.h"
#include "prompt-1.h"
#include "command-2.h"
#include "log-5.h" 
#include "myshrc-9.h"

void sigchld_handler(int signum) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {

        char pname[256] = "Process";

        // lookup pid → name
        for (int i = 0; i < bg_count; i++) {
            if (bg_jobs[i].pid == pid) {
                strcpy(pname, bg_jobs[i].name);

                // remove entry (swap with last)
                bg_jobs[i] = bg_jobs[bg_count - 1];
                bg_count--;
                break;
            }
        }

        char msg[512];
        int len;

        if (WIFEXITED(status)) {
            len = snprintf(msg, sizeof(msg),
                "\n%s exited normally (%d)\n", pname, pid);
        } else {
            len = snprintf(msg, sizeof(msg),
                "\n%s exited abnormally (%d)\n", pname, pid);
        }

        write(STDOUT_FILENO, msg, len);
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