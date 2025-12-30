#ifndef COMMAND_2_H
#define COMMAND_2_H

/**
 * Parses input and executes commands.
 * @param input Raw input line.
 * @param home_dir Shell's home directory (needed for hop).
 */
void process_input(char *input, char *home_dir);

#endif