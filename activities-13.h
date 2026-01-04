#ifndef ACTIVITIES_13_H
#define ACTIVITIES_13_H

#include <sys/types.h>

/**
 * Adds a process to the background tracking list.
 * @param pid The process ID.
 * @param command_name The command string.
 */
void add_process(pid_t pid, char *command_name);

/**
 * Removes a process from the tracking list (called when process terminates).
 * @param pid The process ID to remove.
 * @return 1 if found and removed, 0 otherwise.
 */
int remove_process(pid_t pid);

/**
 * Executes the 'activities' command.
 * Lists running/stopped processes in lexicographic order.
 */
void execute_activities();

#endif