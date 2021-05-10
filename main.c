#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parse.h"
#include "ps.h"
#include "def.h"

char buf[BUF_SIZE];

int main() {
    while (true) {
        print_ps();

        if (fgets(buf, BUF_SIZE, stdin) != NULL) {
            size_t buf_len = strlen(buf);
            if (buf[buf_len - 1] == '\n') buf[buf_len - 1] = '\0';

            Command *commands = malloc(CMD_SIZE * sizeof(Command));
            parse_commands(buf, commands);
            execute_commands(commands);
            free_commands(commands);
            free(commands);
        }
    }
}
