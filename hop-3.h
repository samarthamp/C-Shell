#ifndef HOP_H
#define HOP_H

/**
 * Executes the hop command to change directories.
 * Supports flags: ., .., ~, - and sequential arguments.
 * * @param args Null-terminated array of string arguments (args[0] is "hop").
 * @param home_dir The absolute path of the shell's home directory.
 */
void execute_hop(char **args, char *home_dir);

#endif