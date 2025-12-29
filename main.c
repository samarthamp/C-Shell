#include "header.h"
#include "prompt-1.h"

int main() {
    // 1. Store the directory from which the shell is invoked.
    // This effectively becomes the "~" for this shell session.
    char home_dir[PATH_MAX];
    if (getcwd(home_dir, sizeof(home_dir)) == NULL) {
        perror("Failed to initialize shell home directory");
        return 1;
    }

    char *input_buffer = NULL;
    size_t len = 0;

    // 2. Main Shell Loop
    while (1) {
        // Display requirements from Specification 1
        display_prompt(home_dir);

        // Wait for user input
        // getline automatically allocates memory for input_buffer
        if (getline(&input_buffer, &len, stdin) == -1) {
            // Handle EOF (Ctrl+D) to exit gracefully
            printf("\n");
            break;
        }

        // TODO: In future steps, we will parse and execute the command here.
        // For now, we just loop back to show the prompt again.
    }

    // Cleanup
    free(input_buffer);
    return 0;
}