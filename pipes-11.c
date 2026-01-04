#include "header.h"
#include "pipes-11.h"
#include "ioredir-10.h"
#include "command-2.h" // To call execute_single_command

void execute_pipeline(char *input, int is_bg, char *home_dir, char *prev_dir) {
    // 1. Split by '|'
    char *commands[64]; // Max 64 commands in a pipeline  (can be adjusted)
    int num_cmds = 0;

    char *token = strtok(input, "|");
    while (token != NULL && num_cmds < 63) {
        commands[num_cmds++] = token;
        token = strtok(NULL, "|");
    }
    commands[num_cmds] = NULL;

    if (num_cmds == 0) return;

    // 2. Execution Loop
    int prev_pipe_fd = -1; // Read end of previous pipe
    int pipe_fd[2];

    for (int i = 0; i < num_cmds; i++) {
        // Create pipe for all except the last command
        if (i < num_cmds - 1) {
            if (pipe(pipe_fd) < 0) {
                perror("pipe");
                return;
            }
        }
        
        commands[i][strcspn(commands[i], "\n")] = 0; // Strip newline
        
        // --- OPTIMIZATION FOR SINGLE BUILT-IN COMMAND ---
        // If there is only 1 command (no pipes) and it is a built-in (hop/cd),
        // we MUST run it in the parent. Forking would make 'hop' useless.
        if (num_cmds == 1) {
            // Save IO
            int saved_stdin = dup(STDIN_FILENO);
            int saved_stdout = dup(STDOUT_FILENO);

            // Handle Redirection
            if (handle_redirection(commands[i]) == -1) {
                // Restoration handled implicitly by process exiting usually, but here we are in parent
                restore_io(saved_stdin, saved_stdout);
                return;
            }

            // Execute
            // We need to check if it's external or built-in.
            // We'll modify execute_single_command to return value? 
            // Or just let it run. If it's external, execute_single_command currently forks.
            // We need to change command-2.c to NOT fork if we tell it not to?
            // Actually, execute_single_command in previous steps handled forking internally.
            // We need to refactor command-2.c to separate "parsing/builtin check" from "fork/exec".
            
            // To keep it modular without rewriting everything:
            // We will let command-2.c handle the fork for EXTERNAL commands.
            // But for BUILT-INS, it runs in current process.
            // So calling execute_single_command(..., is_bg=0, ...) is safe IF it handles builtins in-place.
            // My previous implementation of command-2.c handled builtins in parent and returned,
            // and forked only for external. So we are good!
            
            execute_single_command(commands[i], is_bg, home_dir, prev_dir);
            
            // Restore IO
            restore_io(saved_stdin, saved_stdout);
            return;
        }

        // --- PIPELINE (Forking required) ---
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return;
        }

        if (pid == 0) {
            // Child Process
            if (is_bg) setpgid(0, 0);

            // 1. Wiring Input (from previous pipe)
            if (i > 0) {
                dup2(prev_pipe_fd, STDIN_FILENO);
                close(prev_pipe_fd);
            }

            // 2. Wiring Output (to current pipe)
            if (i < num_cmds - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
                close(pipe_fd[0]); // Close read end in writer child
            }

            // 3. Handle Redirection (Overrides pipes)
            if (handle_redirection(commands[i]) == -1) {
                exit(EXIT_FAILURE);
            }

            // 4. Execute
            // Since we are already in a child (pipeline), checking for built-in vs external doesn't matter much for state,
            // but we reuse the logic. 
            // NOTE: 'hop' in a pipe won't affect parent. This is expected behavior.
            execute_single_command(commands[i], 0, home_dir, prev_dir); 
            
            // execute_single_command might return if it was a built-in. 
            // If it was external, it execvp'd.
            exit(0);
        } else {
            // Parent Process
            // Close the read end of the previous pipe (we are done with it)
            if (i > 0) {
                close(prev_pipe_fd);
            }
            // Close the write end of the current pipe (child has it)
            if (i < num_cmds - 1) {
                close(pipe_fd[1]);
                // Save the read end for the next iteration
                prev_pipe_fd = pipe_fd[0];
            }
            
            // Wait behavior:
            // In a pipeline, we typically wait for all, or just the last one?
            // Usually shells wait for all.
            // For simplicity, we can rely on sigchld_handler or wait explicitly.
            if (!is_bg) {
                waitpid(pid, NULL, 0);
            } else {
                printf("[%d] %d\n", i+1, pid);
            }
        }
    }
}