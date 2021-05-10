#include "cmd.h"
#include "stdlib.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define COMMAND_EXIT "exit"

void command_init(Command *command) {
    command->file_in = command->file_append = command->file_out = NULL;
    command->args = NULL;
    command->cmd = NULL;
}

void free_command(Command *command) {
    free(command->file_in);
    free(command->file_out);
    free(command->file_append);
    free(command->cmd);

    for (char **args = command->args; *args; ++args) free(*args);
    free(command->args);
}

void free_commands(Command *commands) {
    while (commands->cmd) free_command(commands++);
}

bool execute_command(Command *command, int fd_in, int fd_out) {
    if (strcmp(command->cmd, COMMAND_EXIT) == 0) exit(0);

    pid_t pid = fork();

    if (pid < 0) {
        return false;
    } else if (pid == 0) {
        if (command->file_in) {
            int in = open(command->file_in, O_RDONLY);
            dup2(in, STDIN_FILENO);
        } else if (fd_in > 0) {
            dup2(fd_in, STDIN_FILENO);
        }

        if (command->file_out) {
            int out = open(command->file_out, O_RDWR | O_CREAT);
            dup2(out, STDOUT_FILENO);
        } else if (command->file_append) {
            int append = open(command->file_append, O_WRONLY | O_CREAT | O_APPEND);
            dup2(append, STDOUT_FILENO);
        } else if (fd_out > 0) {
            dup2(fd_out, STDOUT_FILENO);
        }
        execvp(command->cmd, command->args);
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
    return true;
}

size_t count_commands(Command *commands) {
    Command *start = commands;
    while (commands->cmd) ++commands;
    return commands - start;
}

void execute_commands(Command *commands) {
    size_t cnt_commands = count_commands(commands);

    int fd[2], prev_out_fd = -1;

    for (int i = 0; i < cnt_commands; ++i) {
        pipe(fd);
        execute_command(commands + i, i == 0 ? -1 : prev_out_fd, i == cnt_commands - 1 ? -1 : fd[1]);

        close(fd[1]);
        if (prev_out_fd > 0) close(prev_out_fd);
        prev_out_fd = fd[0];
    }

    if (prev_out_fd > 0) close(prev_out_fd);
}