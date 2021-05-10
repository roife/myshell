#ifndef MYSHELL_CMD_H
#define MYSHELL_CMD_H

#include <stdbool.h>
#include <ntsid.h>

typedef struct {
    char *file_in;
    char *file_out;
    char *file_append;
    char **args;
    char *cmd;
} Command;

void command_init(Command *command);

void free_commands(Command *commands);

void execute_commands(Command *commands);

#endif //MYSHELL_CMD_H
